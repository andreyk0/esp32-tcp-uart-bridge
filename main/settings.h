#ifndef SETTINGS_H
#define SETTINGS_H

#include "esp_err.h"
#include <stdint.h>

// Default values
#define SETTINGS_DEFAULT_PORT 5025
#define SETTINGS_DEFAULT_BAUD 115200
#define SETTINGS_DEFAULT_HOSTNAME "esp32-bridge"

// A struct to hold all our settings
typedef struct {
  uint16_t tcp_port;
  uint32_t uart_baud;
  char hostname[32];
} app_settings_t;

/**
 * @brief Initializes settings by loading from NVS or setting defaults.
 */
void settings_init(void);

/**
 * @brief Saves the current settings to NVS.
 * @return ESP_OK on success, otherwise an error code.
 */
esp_err_t settings_save(void);

/**
 * @brief Gets a pointer to the global settings struct.
 * @return Pointer to the app_settings_t struct.
 */
app_settings_t *settings_get(void);

#endif // SETTINGS_H
