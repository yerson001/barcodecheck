#include "sender.h"
#include "attendance_queue.h"
#include "attendance_data.h"
#include "connectivity.h"
#include "esp_log.h"

#define TAG "SENDER"

void sender_task(void *pv) {
    AttendanceData data;
    while (1) {
        if (xQueueReceive(queue_get_handle_list(), &data, portMAX_DELAY) == pdPASS) {
            if (connectivity_is_online()) {
                ESP_LOGI(TAG, "Enviado al servidor: %s, %s, %s", data.dni, data.type, data.timestamp);
                // Aquí iría tu lógica real de envío
            } else {
                ESP_LOGW(TAG, "Sin internet, guardar en storage (no implementado)");
                // Aquí iría tu lógica de almacenamiento local
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
