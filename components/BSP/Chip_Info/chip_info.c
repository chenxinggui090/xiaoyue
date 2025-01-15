#include "chip_info.h"

#define TAG "Chip_info"

/**
 * @brief       初始化LED
 * @param       无
 * @retval      无
 */
void chip_info(void)
{
    uint32_t flash_size;
    esp_chip_info_t chip_info; /* 定义芯片信息结构体变量 */

    esp_flash_get_size(NULL, &flash_size); /* 获取 FLASH 大小 */
    esp_chip_info(&chip_info);
    ESP_LOGI(TAG, "内核:cup 数量%d",chip_info.cores); /* 获取 CPU 内核数并显示 */
    /* 获取 FLASH 大小并显示 */
    ESP_LOGI(TAG, "FLASH size:%d MB flash",flash_size / (1024 * 1024));
    // 获取内部 RAM 空间大小（单位：字节）
    size_t internal_ram_size = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
    // 获取 PSRAM 空间大小（单位：字节）
    size_t psram_size = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    // 打印内部 RAM 空间大小（单位：KB）
    ESP_LOGI(TAG, "Internal RAM size: %d KB", internal_ram_size / 1024);
    // 打印 PSRAM 空间大小（单位：KB 和 MB）
    ESP_LOGI(TAG, "PSRAM size: %d KB, %d MB", psram_size / 1024, psram_size / (1024 * 1024));
}
