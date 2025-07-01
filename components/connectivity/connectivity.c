#include "connectivity.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#define TAG "CONNECTIVITY"

void init_wifi(void) {
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "UCSP-SALA",
            .password = "S@l@r3u570p",
        },
    };
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();
}


bool connectivity_is_online(void) {
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        // Obtener y mostrar la IP asignada
        esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
        if (netif) {
            esp_netif_ip_info_t ip_info;
            if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
                ESP_LOGI(TAG, "WiFi conectado. IP: " IPSTR, IP2STR(&ip_info.ip));
            } else {
                ESP_LOGW(TAG, "No se pudo obtener la IP");
            }
        } else {
            ESP_LOGW(TAG, "No se pudo obtener la interfaz de red WiFi");
        }
        return true;
    } else {
        ESP_LOGD(TAG, "WiFi NO conectado");
        return false;
    }
}