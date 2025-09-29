#include "settings.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "SETTINGS";
#define NVS_NAMESPACE "config"
#define NVS_KEY "settings"

// Global variable to hold the current settings
static app_settings_t g_settings;

void settings_init(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    } else {
        size_t required_size = sizeof(app_settings_t);
        err = nvs_get_blob(nvs_handle, NVS_KEY, &g_settings, &required_size);
        if (err == ESP_OK && required_size == sizeof(app_settings_t)) {
            ESP_LOGI(TAG, "Successfully loaded settings from NVS");
        } else {
            // Either not found or size mismatch, set to defaults
            if (err == ESP_ERR_NVS_NOT_FOUND) {
                ESP_LOGI(TAG, "Settings not found in NVS. Loading defaults.");
            } else {
                ESP_LOGE(TAG, "NVS error (%s). Loading defaults.", esp_err_to_name(err));
            }
            g_settings.tcp_port = SETTINGS_DEFAULT_PORT;
            g_settings.uart_baud = SETTINGS_DEFAULT_BAUD;
            strncpy(g_settings.hostname, SETTINGS_DEFAULT_HOSTNAME, sizeof(g_settings.hostname));
        }
        nvs_close(nvs_handle);
    }
}

esp_err_t settings_save(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return err;
    }
    
    err = nvs_set_blob(nvs_handle, NVS_KEY, &g_settings, sizeof(app_settings_t));
    if (err == ESP_OK) {
        err = nvs_commit(nvs_handle);
    }

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Successfully saved settings to NVS");
    } else {
        ESP_LOGE(TAG, "Error (%s) saving settings to NVS!", esp_err_to_name(err));
    }

    nvs_close(nvs_handle);
    return err;
}

app_settings_t* settings_get(void) {
    return &g_settings;
}
