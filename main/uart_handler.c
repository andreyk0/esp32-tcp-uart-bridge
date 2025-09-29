#include "driver/gpio.h"
#include "esp_log.h"

#include "uart_handler.h"

static const char *TAG = "UART";

// -- Configuration for UART2
#define UART_TXD (17) // Per datasheet pinout
#define UART_RXD (5)  // Per datasheet pinout

/**
 * @brief Initializes the second UART for SCPI communication
 */
void uart_init(uint32_t baud_rate) {
  uart_config_t uart_config = {
      .baud_rate = baud_rate,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT,
  };
  ESP_ERROR_CHECK(
      uart_driver_install(UART_NUM, UART_BUF_SIZE * 2, 0, 0, NULL, 0));
  ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
  ESP_ERROR_CHECK(uart_set_pin(UART_NUM, UART_TXD, UART_RXD, UART_PIN_NO_CHANGE,
                               UART_PIN_NO_CHANGE));
  ESP_LOGI(TAG, "SCPI UART (UART%d) initialized on TX:%d, RX:%d", UART_NUM,
           UART_TXD, UART_RXD);
}
