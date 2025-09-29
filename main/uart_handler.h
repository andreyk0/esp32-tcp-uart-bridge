#ifndef UART_HANDLER_H
#define UART_HANDLER_H

#include "driver/uart.h"

#define UART_NUM UART_NUM_2
#define UART_BUF_SIZE (1024)

// Modified: Accepts baud rate as a parameter
void uart_init(uint32_t baud_rate);

#endif // UART_HANDLER_H
