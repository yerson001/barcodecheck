#include "sender.h"
#include "attendance_queue.h"
#include "attendance_data.h"
#include "connectivity.h"
#include "esp_log.h"
#include "logger.h"

#define TAG "SENDER"

void sender_task(void *pv)
{
    AttendanceData data;
    while (1)
    {
        if (xQueueReceive(queue_get_handle_list(), &data, portMAX_DELAY) == pdPASS)
        {
            if (connectivity_is_online())
            {
                ESP_LOGI(TAG, "Enviado al servidor: %s, %s, %s", data.dni, data.type, data.timestamp);
                LOG_SEPARATOR();
                LOG_INFO("SENDER  SUCCESS");
                LOG_SEPARATOR();
            }
            else
            {
                ESP_LOGW(TAG, "Sin internet, guardar en storage (no implementado)");
                LOG_SEPARATOR();
                LOG_WARN("SENDER LOCAL");
                LOG_SEPARATOR();
            }
        }
        // UBaseType_t free_stack = uxTaskGetStackHighWaterMark(NULL);
        //  printf("Stack SENDER libre (en palabras): %u\n", free_stack);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
