/******************
 * 头文件
 ******************/
#include "adc_dma.h"
#include "hcalculateNormalization.h"

/******************
 * 条件编译定义
 ******************/
#define TaskGetStack        0
#define Random_Seed         1//模拟随机adc值
#define FREQ_HZ_Read        0

/******************
 * 宏定义
 ******************/
#define TAG "ADC_DMA"
#define TIMES               2000
#define GET_UNIT(x)         ((x>>3) & 0x1)
#define ADC_RESULT_BYTE     4
#define ADC_CONV_LIMIT_EN   0
#define ADC_CONV_MODE       ADC_CONV_SINGLE_UNIT_1
#define ADC_OUTPUT_TYPE     ADC_DIGI_OUTPUT_FORMAT_TYPE2
#define ADC_FREQ_HZ         48000
#define ADC_EVENT_FLAG      (1 << 0) // 定义事件标志位，0 位用于触发事件
#define VoiceNoiceLimit     10//动态底噪取样限制，1s一次

/******************
 * 静态全局变量
 ******************/
static uint32_t adc1_chan_mask = BIT(2);
static uint32_t adc2_chan_mask = 0;
static adc_channel_t channel[1] = {ADC1_CHANNEL_2};
static TaskHandle_t xTaskadcHandle = NULL;
static EventGroupHandle_t adc_event_group;
static int16_t *adc_dma_buff1 = NULL;
static int16_t *adc_dma_buff2 = NULL;
static int16_t *current_buff = NULL;
static int16_t *process_buff = NULL;
static float *process_used_buff = NULL;
static Voice_E voice;
#if FREQ_HZ_Read
    static uint16_t adc_collect_count = 0;
#endif

/******************
 * 静态函数声明
 ******************/
static void continuous_adc_init(uint32_t adc1_chan_mask, uint32_t adc2_chan_mask, 
        adc_channel_t *channel, uint8_t channel_num);
static void adc_dma_collect_task(void *arg);
static void adc_dma_dispose_task(void *arg);

/******************
 * 函数实现
 ******************/
/**
 * @fn      adc配置函数
 * 
 * @brief   continuous_adc_init
 * 
 * @param   adc1_chan_mask：ADC1管教配置
 * @param   adc1_chan_mask：ADC2管教配置
 * @param   channel：通道
 * @param   channel_num：数量
 * 
 * @retval  none
 */
void adc_dma_init(void)
{
    continuous_adc_init(adc1_chan_mask, adc2_chan_mask, channel, sizeof(channel) / sizeof(adc_channel_t));
    adc_digi_start();
    // 创建事件组
    adc_event_group = xEventGroupCreate();

    xTaskCreatePinnedToCore(adc_dma_collect_task, "adc_dma_collect_task", 1024 * 3, NULL, 3,&xTaskadcHandle, 1);
    xTaskCreatePinnedToCore(adc_dma_dispose_task, "adc_dma_dispose_task", 1024 * 3, NULL, 4,NULL, 1);
}

/**
 * @fn      adc配置函数
 * 
 * @brief   continuous_adc_init
 * 
 * @param   adc1_chan_mask：ADC1管教配置
 * @param   adc1_chan_mask：ADC2管教配置
 * @param   channel：通道
 * @param   channel_num：数量
 * 
 * @retval  none
 */
static void continuous_adc_init(uint32_t adc1_chan_mask, uint32_t adc2_chan_mask, 
        adc_channel_t *channel, uint8_t channel_num)
{
    adc_digi_init_config_t adc_dma_config = {
        .max_store_buf_size = 4096,
        .conv_num_each_intr = TIMES,
        .adc1_chan_mask = adc1_chan_mask,
        .adc2_chan_mask = adc2_chan_mask,
    };
    ESP_ERROR_CHECK(adc_digi_initialize(&adc_dma_config));

    adc_digi_configuration_t dig_cfg = {
        .conv_limit_en = ADC_CONV_LIMIT_EN,
        .conv_limit_num = 250,
        .sample_freq_hz = ADC_FREQ_HZ,
        .conv_mode = ADC_CONV_MODE,
        .format = ADC_OUTPUT_TYPE,
    };

    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    dig_cfg.pattern_num = channel_num;
    for (int i = 0; i < channel_num; i++) {
        uint8_t unit = GET_UNIT(channel[i]);
        uint8_t ch = channel[i] & 0x7;
        adc_pattern[i].atten = ADC_ATTEN_DB_11;
        adc_pattern[i].channel = ch;
        adc_pattern[i].unit = unit;
        adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;

        ESP_LOGI(TAG, "adc_pattern[%d].atten is :%x", i, adc_pattern[i].atten);
        ESP_LOGI(TAG, "adc_pattern[%d].channel is :%x", i, adc_pattern[i].channel);
        ESP_LOGI(TAG, "adc_pattern[%d].unit is :%x", i, adc_pattern[i].unit);
    }
    dig_cfg.adc_pattern = adc_pattern;
    ESP_ERROR_CHECK(adc_digi_controller_configure(&dig_cfg));
}

/**
 * @fn      adc采集任务
 * 
 * @brief   adc_dma_collect_task
 * 
 * @param   arg:传递数据
 * 
 * @retval  none
 */
static void adc_dma_collect_task(void *arg)
{
    esp_err_t ret;
    uint32_t ret_num = 0;
    uint8_t *result = NULL;
    uint16_t adc_dma_buff_count = 0;
    // 申请内存
    result = (uint8_t*)malloc(TIMES * sizeof(uint8_t));
    if (result == NULL) {
        ESP_LOGE(TAG, "Memory allocation failed!");
        vTaskDelete(NULL);
        return;
    }
    adc_dma_buff1 = (int16_t *)heap_caps_malloc(ADC_FREQ_HZ * sizeof(int16_t), MALLOC_CAP_SPIRAM);
    adc_dma_buff2 = (int16_t *)heap_caps_malloc(ADC_FREQ_HZ * sizeof(int16_t), MALLOC_CAP_SPIRAM);
    if (adc_dma_buff1 == NULL || adc_dma_buff2 == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for buffers!");
        free(result);  // 释放之前申请的内存
        vTaskDelete(NULL);
        return;
    }
    current_buff = adc_dma_buff1;
    process_buff = adc_dma_buff2;
    // 初始化result
    memset(result, 0xcc, TIMES);

#if Random_Seed
    int16_t random_num = esp_random() % 200; 
#endif
    while(1) {
        ret = adc_digi_read_bytes(result, TIMES, &ret_num, ADC_MAX_DELAY);
        if (ret == ESP_OK || ret == ESP_ERR_INVALID_STATE) {
            for (int i = 0; i < ret_num; i += ADC_RESULT_BYTE)
            {
#if Random_Seed
                current_buff[adc_dma_buff_count++] = random_num;
#else
                adc_digi_output_data_t *p = (void *)&result[i];
                current_buff[adc_dma_buff_count++] = p->type2.data;             
#endif
#if FREQ_HZ_Read
                adc_collect_count++;
#endif
                if(adc_dma_buff_count>=ADC_FREQ_HZ){
                    adc_dma_buff_count = 0;
                    if (current_buff == adc_dma_buff1) {
                        current_buff = adc_dma_buff2;
                        process_buff = adc_dma_buff1;
                    } else {
                        current_buff = adc_dma_buff1;
                        process_buff = adc_dma_buff2;
                    }
#if Random_Seed
                    random_num = esp_random() % 200; 
#endif
                    
#if TaskGetStack
                    UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(NULL);
                    ESP_LOGI(__FUNCTION__, "Task high water mark: %d", highWaterMark);
#endif
                    // 设置事件标志位，通知 adc_dma_dispose_task 执行事件
                    xEventGroupSetBits(adc_event_group, ADC_EVENT_FLAG);
                }
            }
        }
        vTaskDelay(5);
    }
    // 释放内存
    free(result);
    int16_t *temp_buff1 = (int16_t *)adc_dma_buff1;
    int16_t *temp_buff2 = (int16_t *)adc_dma_buff2;
    free(temp_buff1);
    free(temp_buff2);
    adc_digi_stop();
    ret = adc_digi_deinitialize();
    assert(ret == ESP_OK);
}

/**
 * @fn      adc处理任务
 * 
 * @brief   adc_dma_dispose_task
 * 
 * @param   arg:传递数据
 * 
 * @retval  none
 */
static void adc_dma_dispose_task(void *arg)
{
    double voice_noise_temp = 0,voice_noise = 0;
    uint8_t VoiceCount = 0;
    int Crack=0;
    while(1) {
        // 等待事件标志位 ADC_EVENT_FLAG 被设置
        EventBits_t bits = xEventGroupWaitBits(adc_event_group, ADC_EVENT_FLAG, pdTRUE, pdFALSE, portMAX_DELAY);

        // 检查事件标志位是否被触发
        if (bits & ADC_EVENT_FLAG) {
#if FREQ_HZ_Read
    ESP_LOGI(TAG, "adc_collect_count:%d",adc_collect_count);
    adc_collect_count=0;
#endif
            switch(voice) {
                case GET_Noise:
                    voice_noise_temp = calculateNoise(process_buff,ADC_FREQ_HZ);
                    if(voice_noise_temp!=0.0){
                        voice_noise = (voice_noise_temp + voice_noise)/2;
                    } else {
                        voice_noise = voice_noise_temp;
                    }
                    VoiceCount++;
                    if(VoiceCount>=VoiceNoiceLimit){
                        voice = GET_Voice_data;
                        voice_noise_temp = 0;
                        voice_noise = 0;
                        process_used_buff = (float *)heap_caps_malloc(ADC_FREQ_HZ * sizeof(float), MALLOC_CAP_SPIRAM);
                    }
                    ESP_LOGI(TAG,"voice_noise:%f",voice_noise);
                break;
                case GET_Voice_data:
                    {
                        float sum = process_buff[0] * NormalizationFactor;
                        int crack_flag = 0;
                        for (size_t i = 0; i < ADC_FREQ_HZ; ++i)
                        {
                            process_used_buff[i] = process_buff[i] * NormalizationFactor;
                            sum = (sum + fabs(process_used_buff[i]))/2;
                            if (process_used_buff[i] > voice_noise*3) {
                                crack_flag = 1;
                            }
                        }

                        if (crack_flag) {
                            crack_flag=0;
                            if (sum < voice_noise*ADC_FREQ_HZ*0.5) {
                                Crack++;
                            }
                        }
                        ESP_LOGI(TAG,"sum:%f,voice_noise*ADC_FREQ_HZ*0.5:%f",sum,voice_noise*ADC_FREQ_HZ*0.5);
                    }
                break;
                case Deal_over:
                    free(process_used_buff);
                    Crack=0;
                    voice=GET_Noise;
                break;
            }
#if TaskGetStack
                    UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(NULL);
                    ESP_LOGI(__FUNCTION__, "Task high water mark: %d", highWaterMark);
#endif
        }
        vTaskDelay(50);
    }
}

/**
 * @fn      adc处理任务状态转换
 * 
 * @brief   adc_dma_dispose_task_state_change
 * 
 * @param   none
 * 
 * @retval  none
 */
void adc_dma_dispose_task_state_change(Voice_E state)
{
    voice = state;
}