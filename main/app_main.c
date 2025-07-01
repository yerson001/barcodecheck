#include <stdio.h>
#include "gm75_scanner.h"
#include "esp_log.h"
#include "logger.h"

void app_main(void) {
    ESP_LOGI("APP", "Iniciando ColeCheck Scanner...");
    LOG_SEPARATOR();
    LOG_INFO("hola");
    LOG_SEPARATOR();
    gm75_scanner_init();
    gm75_scanner_start();
}
