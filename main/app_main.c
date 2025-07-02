#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "logger.h"
#include "connectivity.h"
#include "sender.h"
#include "gm75_scanner.h"
#include "esp_heap_caps.h"
#include "attendance_queue.h"




/*
*******************************************************************************
# ESP-IDF Partition Table
# Name, Type, SubType, Offset, Size, Flags
nvs,data,nvs,0x9000,24K,
phy_init,data,phy,0xf000,4K,
factory,app,factory,0x10000,1M,
*******************************************************************************
*/

void app_main(void)
{
    init_wifi();
    connectivity_is_online();
    queue_init();
    xTaskCreate(scanner_test, "scanner_task", 4096, NULL, 5, NULL);
    xTaskCreate(sender_task, "sender_task", 4096, NULL, 5, NULL);

    ESP_LOGI("RAM", "RAM libre: %d bytes", heap_caps_get_free_size(MALLOC_CAP_8BIT));
}