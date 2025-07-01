#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "attendance_data.h"
#include "esp_log.h"
#include "attendance_queue.h"
#include "logger.h"
#include "esp_random.h"
#include "connectivity.h"
#include "sender.h"

#define TAG "SCANNER_TASK"

void scanner_task(void *pv) {
    int count = 0;
    while (1) {
        AttendanceData data;
        snprintf(data.dni, sizeof(data.dni), "DNI%03d", count);
        strcpy(data.type, (count % 2 == 0) ? "entrance" : "exit");
        snprintf(data.timestamp, sizeof(data.timestamp), "2025-07-01 15:%02d:00", count % 60);

        if (xQueueSend(queue_get_handle_list(), &data, pdMS_TO_TICKS(1000)) == pdPASS) {
            LOG_SEPARATOR();
            ESP_LOGI(TAG, "Enviado: %s, %s, %s", data.dni, data.type, data.timestamp);
            LOG_SEPARATOR();
        } else {
            ESP_LOGW(TAG, "Cola llena, no se pudo enviar");
        }
        count++;
        // Retardo aleatorio entre 2000 ms (2s) y 6000 ms (6s)
        uint32_t delay_ms = 2000 + (esp_random() % 4001); // 4001 para incluir hasta 6000
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

void app_main(void) {
    init_wifi();
    connectivity_is_online();
    queue_init();
    xTaskCreate(scanner_task, "scanner_task", 2048, NULL, 5, NULL);
    xTaskCreate(sender_task, "sender_task", 4096, NULL, 5, NULL);
}