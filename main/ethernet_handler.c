#include "ethernet_handler.h"
#include "driver/gpio.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "ETHERNET";

// WT32-ETH01 specific configuration
#define ETH_PHY_POWER_PIN 16

/**
 * @brief Event handler for Ethernet events
 */
static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data) {
  uint8_t mac_addr[6] = {0};
  /* we can get the ethernet driver handle from event data */
  esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

  switch (event_id) {
  case ETHERNET_EVENT_CONNECTED:
    // VERIFIED: This matches the original code exactly.
    esp_netif_get_mac(eth_handle, mac_addr);
    ESP_LOGI(TAG, "Ethernet Link Up");
    ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0],
             mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    break;
  case ETHERNET_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "Ethernet Link Down");
    break;
  case ETHERNET_EVENT_START:
    ESP_LOGI(TAG, "Ethernet Started");
    break;
  case ETHERNET_EVENT_STOP:
    ESP_LOGI(TAG, "Ethernet Stopped");
    break;
  default:
    break;
  }
}

/**
 * @brief Event handler for IP_EVENT_ETH_GOT_IP
 */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data) {
  ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
  const esp_netif_ip_info_t *ip_info = &event->ip_info;

  ESP_LOGI(TAG, "Ethernet Got IP Address");
  ESP_LOGI(TAG, "~~~~~~~~~~~");
  ESP_LOGI(TAG, "ETH IP: " IPSTR, IP2STR(&ip_info->ip));
  ESP_LOGI(TAG, "ETH MASK: " IPSTR, IP2STR(&ip_info->netmask));
  ESP_LOGI(TAG, "ETH GW: " IPSTR, IP2STR(&ip_info->gw));
  ESP_LOGI(TAG, "~~~~~~~~~~~");
}

void ethernet_init(const char *hostname) {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
  esp_netif_t *eth_netif = esp_netif_new(&cfg);

  esp_netif_set_hostname(eth_netif, hostname);

  gpio_reset_pin(ETH_PHY_POWER_PIN);
  gpio_set_direction(ETH_PHY_POWER_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(ETH_PHY_POWER_PIN, 1);
  vTaskDelay(pdMS_TO_TICKS(100));

  eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
  eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
  phy_config.phy_addr = 1;
  phy_config.reset_gpio_num = -1;

  eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
  esp32_emac_config.smi_gpio.mdc_num = 23;
  esp32_emac_config.smi_gpio.mdio_num = 18;
  esp32_emac_config.interface = EMAC_DATA_INTERFACE_RMII;
  esp32_emac_config.clock_config.rmii.clock_mode = EMAC_CLK_EXT_IN;
  esp32_emac_config.clock_config.rmii.clock_gpio = EMAC_CLK_IN_GPIO;

  esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);
  esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);
  esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
  esp_eth_handle_t eth_handle = NULL;
  ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle));

  // Attach the ethernet driver to the network interface
  esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle));

  // Register event handlers
  ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID,
                                             &eth_event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP,
                                             &got_ip_event_handler, NULL));

  ESP_ERROR_CHECK(esp_eth_start(eth_handle));
}
