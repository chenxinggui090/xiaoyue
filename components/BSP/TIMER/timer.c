#include "timer.h"

#define TAG "TIMER"

/******************
 * 函数实现
 ******************/
/**
 * @fn      定时器初始化
 * 
 * @brief   timer_init
 * 
 * @param   none
 * 
 * @retval  none
 */
void timer_init(void)
{
    // // 设置定时器为定时模式
    // timer_config_t config = {
    //     .alarm_en = TIMER_ALARM_EN,
    //     .auto_reload = true,
    //     .divider = 80, // 定时器时钟为 80 MHz，所以需要将其除以 80 得到 1 微秒的单位
    //     .counter_dir = TIMER_COUNT_UP,
    //     .counter_en = TIMER_START,
    //     .intr_type = TIMER_INTR_LEVEL
    // };
    // timer_init(TIMER_GROUP_0, TIMER_0, &config);

    // // 设置定时器的触发时间为 20.83 微秒
    // timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 20.83);

    // // 启动定时器
    // timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    // timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_isr, NULL, ESP_INTR_FLAG_IRAM, NULL);
    // timer_start(TIMER_GROUP_0, TIMER_0);
}
