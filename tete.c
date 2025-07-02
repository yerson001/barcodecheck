#include "wifi_manager.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"
#include <string.h>

static const char *TAG = "wifi_manager";
static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static int retry_count = 0;
static const int MAX_RETRY = 5;
static char wifi_ssid[32];
static char wifi_pass[64];

static esp_netif_t *sta_netif = NULL;
static esp_event_handler_instance_t wifi_hdlr, ip_hdlr;

esp_err_t wifi_manager_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    esp_netif_init();
    esp_event_loop_create_default();
    esp_wifi_set_default_wifi_sta_handlers();
    sta_netif = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_manager_wifi_event, NULL, &wifi_hdlr));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, ESP_EVENT_ANY_ID, &wifi_manager_ip_event, NULL, &ip_hdlr));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    return ESP_OK;
}

esp_err_t wifi_manager_set_credentials(const char *ssid, const char *pass)
{
    strncpy(wifi_ssid, ssid, sizeof(wifi_ssid) - 1);
    strncpy(wifi_pass, pass, sizeof(wifi_pass) - 1);
    return ESP_OK;
}

esp_err_t wifi_manager_connect(void)
{
    if (!strlen(wifi_ssid))
        return ESP_ERR_INVALID_ARG;

    wifi_config_t wcfg = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    strcpy((char *)wcfg.sta.ssid, wifi_ssid);
    strcpy((char *)wcfg.sta.password, wifi_pass);

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wcfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE, pdFALSE, pdMS_TO_TICKS(30000));

    return (bits & WIFI_CONNECTED_BIT) ? ESP_OK : ESP_FAIL;
}

esp_err_t wifi_manager_disconnect(void)
{
    return esp_wifi_disconnect();
}

esp_err_t wifi_manager_deinit(void)
{
    esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_hdlr);
    esp_event_handler_instance_unregister(IP_EVENT, ESP_EVENT_ANY_ID, ip_hdlr);
    vEventGroupDelete(wifi_event_group);
    esp_wifi_stop();
    esp_wifi_deinit();
    esp_netif_destroy(sta_netif);
    return ESP_OK;
}

bool wifi_manager_is_connected(void)
{
    EventBits_t bits = xEventGroupGetBits(wifi_event_group);
    return (bits & WIFI_CONNECTED_BIT);
}

esp_err_t wifi_manager_get_ip(char *out_ip_str, size_t len)
{
    esp_netif_ip_info_t ip_info;
    esp_err_t ret = esp_netif_get_ip_info(sta_netif, &ip_info);
    if (ret != ESP_OK)
        return ret;
    snprintf(out_ip_str, len, IPSTR, IP2STR(&ip_info.ip));
    return ESP_OK;
}

// — Event callbacks —

static void wifi_manager_wifi_event(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    if (id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGW(TAG, "Wi-Fi lost connection");
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        if (retry_count < MAX_RETRY)
        {
            esp_wifi_connect();
            retry_count++;
            ESP_LOGI(TAG, "Retry #%d", retry_count);
        }
        else
        {
            xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
        }
    }
}

static void wifi_manager_ip_event(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    if (id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        retry_count = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

#include "wifi_manager.h"
#include "esp_log.h"

void app_main(void)
{
    wifi_manager_init();

    wifi_manager_set_credentials("MiRedWiFi", "MiClave123");
    if (wifi_manager_connect() == ESP_OK)
    {
        ESP_LOGI("MAIN", "Conectado!");
        char ip[16];
        wifi_manager_get_ip(ip, sizeof(ip));
        ESP_LOGI("MAIN", "Mi IP es %s", ip);
    }
    else
    {
        ESP_LOGE("MAIN", "No pude conectarme 😢");
    }

    // Loop principal
    while (true)
    {
        if (!wifi_manager_is_connected())
        {
            ESP_LOGW("MAIN", "Desconectado, intentando reconectar...");
            wifi_manager_connect();
        }
        vTaskDelay(pdMS_TO_TICKS(10000));
    }

    // Si fuera necesario, puedes desconectar/desinicializar:
    // wifi_manager_disconnect();
    // wifi_manager_deinit();
}
