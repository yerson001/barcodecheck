#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "gm75_scanner.h"
#include "esp_random.h"
#include "attendance_data.h"
#include "attendance_queue.h"
#include "logger.h"

#define TAG "GM75_SCANNER"
#define UART_NUM_GM75 UART_NUM_2
#define BUF_SIZE 1024
#define GM75_RX_PIN GPIO_NUM_13
#define GM75_TX_PIN GPIO_NUM_14

static void gm75_task(void *pvParameters);

void gm75_scanner_init(void)
{
    uart_config_t config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};

    uart_param_config(UART_NUM_GM75, &config);
    uart_set_pin(UART_NUM_GM75, GM75_TX_PIN, GM75_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_GM75, BUF_SIZE * 2, 0, 0, NULL, 0);

    ESP_LOGI(TAG, "UART2 para GM75 inicializada");
}

void gm75_scanner_start(void)
{
    xTaskCreate(gm75_task, "gm75_task", 4096, NULL, 5, NULL);
}

static void gm75_task(void *pvParameters)
{
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);

    while (1)
    {
        int len = uart_read_bytes(UART_NUM_GM75, data, BUF_SIZE - 1, pdMS_TO_TICKS(1000));

        if (len > 0)
        {
            data[len] = '\0';
            for (int i = 0; i < len; i++)
            {
                if (data[i] == '\r' || data[i] == '\n')
                {
                    data[i] = '\0';
                    break;
                }
            }

            if (strlen((char *)data) > 0)
            {
                ESP_LOGI(TAG, "Código escaneado: %s", data);
                char *descifrado = cesar_decrypt((const char *)data, 3);
                if (descifrado)
                {
                    printf("QR desencriptado: %s\n", descifrado);
                    free(descifrado);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    free(data);
}

void scanner_test(void *pv)
{
    int count = 0;
    while (1)
    {
        AttendanceData data;
        snprintf(data.dni, sizeof(data.dni), "DNI%03d", count);
        strcpy(data.type, (count % 2 == 0) ? "entrance" : "exit");
        snprintf(data.timestamp, sizeof(data.timestamp), "2025-07-01 15:%02d:00", count % 60);

        if (xQueueSend(queue_get_handle_list(), &data, pdMS_TO_TICKS(1000)) == pdPASS)
        {
            LOG_SEPARATOR();
            LOG_INFO("GM75SCANNER");
            ESP_LOGI(TAG, "Enviado: %s, %s, %s", data.dni, data.type, data.timestamp);
            LOG_SEPARATOR();
        }
        else
        {
            ESP_LOGW(TAG, "Cola llena, no se pudo enviar");
        }
        count++;
        // UBaseType_t free_stack = uxTaskGetStackHighWaterMark(NULL);
        //  printf("Stack libre SCANNER (en palabras): %u\n", free_stack);
        //   Retardo aleatorio entre 2000 ms (2s) y 6000 ms (6s)
        uint32_t delay_ms = 2000 + (esp_random() % 4001); // 4001 para incluir hasta 6000
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}