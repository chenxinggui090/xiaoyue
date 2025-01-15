/******************
 * 头文件
 ******************/
#include <stdio.h>
#include "esp_log.h"

#include "chip_info.h"
#include "adc_dma.h"

/******************
 * 宏定义
 ******************/
#define Device_heap_caps_info   1
#define TAG "heap_caps"

/******************
 * 函数实现
 ******************/
/**
 * @fn      函数入口
 * 
 * @brief   app_main
 * 
 * @param   none
 * 
 * @retval  none
 */
void app_main(void)
{
    esp_err_t ret;
    /* 初始化 NVS */
    ret = nvs_flash_init(); 
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    /* 获取芯片信息 */
    chip_info();
    /* adc初始化 */
    adc_dma_init();
    while(1)
    {
#if Device_heap_caps_info
    ESP_LOGI(TAG, "INTERNAL:%d,SPIRAM:%d", 
                heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
                heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
#endif
        vTaskDelay(5000);
    }
}
