#ifndef _ADC_DAM_H
#define _ADC_DAM_H

/******************
 * 头文件
 ******************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/adc.h"
#include "esp_log.h"
#include "esp_system.h"

/******************
 * 重定义
 ******************/
typedef enum {
    GET_Noise,//动态底噪处理
    GET_Voice_data,//音频数据处理
    Deal_over,//处理结束恢复默认
} Voice_E;

/******************
 * 函数声明
 ******************/
void adc_dma_init(void);
void adc_dma_dispose_task_state_change(Voice_E state);

#endif