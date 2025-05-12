#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "./packages/pwm/pwm.h"
#include "./packages/uart/uart.h"
#include "./packages/ihm/ihm.h"


#define CHIP_NAME "/dev/gpiochip0"
#define LINE_NUMBER 12 // Change this to the appropriate GPIO line number

#define MAX_MESSAGE_LENGTH 100

int main() {

    UART_param uart_param = {
        .device = "/dev/ttyAMA0", 
        .baud = 115200,          
        .end_line = '\n',      
        .message_max_length = MAX_MESSAGE_LENGTH,
        .timeout = 500,
    };

    UART* uart = uart_new(&uart_param);
    if (uart == NULL) {
        fprintf(stderr, "Failed to initialize UART\n");
        return 1;
    }

    if (uart_start(uart) == -1) {
        fprintf(stderr, "Failed to start UART\n");
        uart_free(uart);
        return 1;
    }
    printf("UART started successfully.\n");

    // Start interface menu
    ihm_menu(uart);

    // Clean up 
    uart_stop(uart);
    uart_free(uart);
    return 0;
}