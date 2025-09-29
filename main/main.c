#include "nvs_flash.h"
#include "esp_system.h"

#include "ethernet_handler.h"
#include "uart_handler.h"
#include "tcp_server.h"
#include "settings.h"
#include "cli.h"

void app_main(void) {
    // Initialize NVS
    ESP_ERROR_CHECK(nvs_flash_init());
    
    // Load settings from NVS or use defaults
    settings_init();
    app_settings_t* settings = settings_get();

    // Initialize application modules with loaded settings
    ethernet_init(settings->hostname);
    uart_init(settings->uart_baud);

    // Start the business logic
    tcp_server_start(settings->tcp_port);

    // Start the command-line interface
    cli_init();
}
