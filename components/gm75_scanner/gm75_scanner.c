#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "gm75_scanner.h"

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
                char *descifrado = cesar_decrypt((const char*)data, 3); 
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
