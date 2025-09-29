#include "cli.h"
#include "argtable3/argtable3.h"
#include "esp_console.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "settings.h"
#include <string.h>

static const char *TAG = "CLI";

/* 'set_hostname' command */
static struct {
  struct arg_str *hostname;
  struct arg_end *end;
} set_hostname_args;

static int do_set_hostname_cmd(int argc, char **argv) {
  int nerrors = arg_parse(argc, argv, (void **)&set_hostname_args);
  if (nerrors != 0) {
    arg_print_errors(stderr, set_hostname_args.end, argv[0]);
    return 1;
  }
  strncpy(settings_get()->hostname, set_hostname_args.hostname->sval[0],
          sizeof(settings_get()->hostname) - 1);
  settings_get()->hostname[sizeof(settings_get()->hostname) - 1] =
      '\0'; // Ensure null termination
  printf("OK. Hostname set to: %s\n", settings_get()->hostname);
  return 0;
}

/* 'set_port' command */
static struct {
  struct arg_int *port;
  struct arg_end *end;
} set_port_args;

static int do_set_port_cmd(int argc, char **argv) {
  int nerrors = arg_parse(argc, argv, (void **)&set_port_args);
  if (nerrors != 0) {
    arg_print_errors(stderr, set_port_args.end, argv[0]);
    return 1;
  }
  settings_get()->tcp_port = set_port_args.port->ival[0];
  printf("OK. Port set to: %d\n", settings_get()->tcp_port);
  return 0;
}

/* 'set_baud' command */
static struct {
  struct arg_int *baud;
  struct arg_end *end;
} set_baud_args;

static int do_set_baud_cmd(int argc, char **argv) {
  int nerrors = arg_parse(argc, argv, (void **)&set_baud_args);
  if (nerrors != 0) {
    arg_print_errors(stderr, set_baud_args.end, argv[0]);
    return 1;
  }
  settings_get()->uart_baud = set_baud_args.baud->ival[0];
  printf("OK. Baud rate set to: %lu\n", settings_get()->uart_baud);
  return 0;
}

/* 'get_config' command */
static int do_get_config_cmd(int argc, char **argv) {
  app_settings_t *s = settings_get();
  printf("Current Configuration:\n");
  printf("  Hostname: %s\n", s->hostname);
  printf("  TCP Port: %d\n", s->tcp_port);
  printf("  UART Baud: %lu\n", s->uart_baud);
  return 0;
}

/* 'save' command */
static int do_save_cmd(int argc, char **argv) {
  esp_err_t err = settings_save();
  if (err == ESP_OK) {
    printf("Settings saved successfully.\n");
  } else {
    printf("Failed to save settings.\n");
  }
  return 0;
}

/* 'reboot' command */
static int do_reboot_cmd(int argc, char **argv) {
  printf("Rebooting in 2 seconds...\n");
  vTaskDelay(pdMS_TO_TICKS(2000));
  esp_restart();
  return 0;
}

static void register_commands(void) {
  set_hostname_args.hostname =
      arg_str1(NULL, NULL, "<hostname>", "Hostname for the device");
  set_hostname_args.end = arg_end(20);
  const esp_console_cmd_t set_hostname_cmd = {.command = "set_hostname",
                                              .help =
                                                  "Set the network hostname",
                                              .hint = NULL,
                                              .func = &do_set_hostname_cmd,
                                              .argtable = &set_hostname_args};
  ESP_ERROR_CHECK(esp_console_cmd_register(&set_hostname_cmd));

  set_port_args.port =
      arg_int1(NULL, NULL, "<port>", "TCP port for the bridge");
  set_port_args.end = arg_end(20);
  const esp_console_cmd_t set_port_cmd = {.command = "set_port",
                                          .help = "Set the TCP server port",
                                          .hint = NULL,
                                          .func = &do_set_port_cmd,
                                          .argtable = &set_port_args};
  ESP_ERROR_CHECK(esp_console_cmd_register(&set_port_cmd));

  set_baud_args.baud =
      arg_int1(NULL, NULL, "<baud>", "Baud rate for the bridged UART");
  set_baud_args.end = arg_end(20);
  const esp_console_cmd_t set_baud_cmd = {.command = "set_baud",
                                          .help =
                                              "Set the bridged UART baud rate",
                                          .hint = NULL,
                                          .func = &do_set_baud_cmd,
                                          .argtable = &set_baud_args};
  ESP_ERROR_CHECK(esp_console_cmd_register(&set_baud_cmd));

  const esp_console_cmd_t get_config_cmd = {
      .command = "get_config",
      .help = "Get the current configuration",
      .hint = NULL,
      .func = &do_get_config_cmd,
  };
  ESP_ERROR_CHECK(esp_console_cmd_register(&get_config_cmd));

  const esp_console_cmd_t save_cmd = {
      .command = "save",
      .help = "Save the current settings to flash",
      .hint = NULL,
      .func = &do_save_cmd,
  };
  ESP_ERROR_CHECK(esp_console_cmd_register(&save_cmd));

  const esp_console_cmd_t reboot_cmd = {
      .command = "reboot",
      .help = "Reboot the device",
      .hint = NULL,
      .func = &do_reboot_cmd,
  };
  ESP_ERROR_CHECK(esp_console_cmd_register(&reboot_cmd));
}

void cli_init(void) {
  esp_console_repl_t *repl = NULL;
  esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
  repl_config.prompt = "bridge> ";
  repl_config.max_cmdline_length = 64;

  esp_console_dev_uart_config_t uart_config =
      ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));

  register_commands();

  ESP_ERROR_CHECK(esp_console_register_help_command());

  // Print a startup banner.
  printf("\n--- TCP-UART Bridge CLI ---\n");
  printf("Enter a command. Use 'help' to see this list.\n");
  printf("After changing settings, use 'save' and 'reboot' to apply.\n\n");

  ESP_LOGI(TAG, "Starting REPL...");
  ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
