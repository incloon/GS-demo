#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "esp_http_client.h"
#include "esp_https_ota.h"

#include "wifi_provisioning/manager.h"
#include "wifi_provisioning/scheme_ble.h"

static const char *TAG = "OTA_BLE";
static EventGroupHandle_t wifi_event_group;
static const int WIFI_CONNECTED_BIT = BIT0;

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_STA_START) {
            esp_wifi_connect();
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            ESP_LOGI(TAG, "Disconnected from AP, retrying...");
            esp_wifi_connect();
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "Got IP address");
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_base == WIFI_PROV_EVENT) {
        if (event_id == WIFI_PROV_CRED_SUCCESS) {
            ESP_LOGI(TAG, "Provisioning successful");
        } else if (event_id == WIFI_PROV_END) {
            ESP_LOGI(TAG, "Stopping provisioning");
            wifi_prov_mgr_deinit();
        }
    }
}

static void start_ble_provisioning(void)
{
    wifi_prov_mgr_config_t config = {
        .scheme = wifi_prov_scheme_ble,
        .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_NONE,
    };

    ESP_ERROR_CHECK(wifi_prov_mgr_init(config));

    uint8_t eth_mac[6];
    esp_read_mac(eth_mac, ESP_MAC_WIFI_STA);
    char service_name[20];
    snprintf(service_name, sizeof(service_name), "PROV_%02X%02X%02X",
             eth_mac[3], eth_mac[4], eth_mac[5]);

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID,
                                               &event_handler, NULL));

    ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(WIFI_PROV_SECURITY_1, NULL,
                                                     service_name, NULL));

    ESP_LOGI(TAG, "Provisioning started with service name %s", service_name);
    ESP_LOGI(TAG, "Use the ESP Provisioning app to connect and provide Wi-Fi credentials");
}

static void perform_ota_update(void)
{
    ESP_LOGI(TAG, "Checking for OTA update");
    esp_http_client_config_t config = {
        .url = "https://example.com/firmware.bin",
        .cert_pem = NULL,
    };
    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };
    esp_https_ota(&ota_config);
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_event_group = xEventGroupCreate();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    start_ble_provisioning();

    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT,
                        pdFALSE, pdTRUE, portMAX_DELAY);

    perform_ota_update();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

