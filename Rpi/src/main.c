#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#include "./packages/pwm/pwm.h"
#include "./packages/uart/uart.h"
#include "./packages/ihm/ihm.h"
#include "./packages/automatic/automatic.h"
#include "./packages/logger/logger.h"

#include "./utils.h"

#define CHIP_NAME "/dev/gpiochip0"
#define LINE_NUMBER 12 // Change this to the appropriate GPIO line number

#define MAX_MESSAGE_LENGTH 100

// Global pointers for cleanup in signal handler
static UART *g_uart = NULL;
static PWM *g_pwm = NULL;

void cleanup_and_exit(int code) {
    LOG_INFO("Cleaning up...");
    if (g_uart) {
        uart_stop(g_uart);
        uart_free(g_uart);
        g_uart = NULL;
    }
    if (g_pwm) {
        pwm_stop(g_pwm);
        pwm_free(g_pwm);
        g_pwm = NULL;
    }
    LOG_INFO("Cleanup completed successfully");
    LOG_INFO("Exiting the program...");
    exit(code);
}

void sigint_handler(int signum) {
    printf("\n\n");
    LOG_INFO("Caught SIGINT (Ctrl+C)");
    cleanup_and_exit(0);
}

int main() {
    // Register SIGINT handler
    signal(SIGINT, sigint_handler);

    LOG_INFO("Starting the program...");

    LOG_INFO("Initializing UART...");
    UART_param uart_param = {
        .device = "/dev/ttyAMA0", 
        .baud = 115200,          
        .end_line = '\n',      
        .message_max_length = MAX_MESSAGE_LENGTH,
        .timeout = 500,
    };

    g_uart = uart_new(&uart_param);
    if (g_uart == NULL) {
        LOG_ERROR("Failed to initialize UART");
        return 1;
    }

    if (uart_start(g_uart) == -1) {
        LOG_ERROR("Failed to start UART");
        uart_free(g_uart);
        g_uart = NULL;
        return 1;
    }
    LOG_INFO("UART initialized successfully");

    // Initialize the PWM device
    LOG_INFO("Initializing PWM...");
    g_pwm = pwm_new(CHIP_NAME, LINE_NUMBER);
    if (!g_pwm) {
        LOG_ERROR("Failed to initialize PWM");
        cleanup_and_exit(1);
    }
    LOG_INFO("PWM initialized successfully");

    // Init the automatic mode
    automatic_init(g_uart, g_pwm);

    // Start interface menu
    LOG_INFO("Starting the interface menu...\n");

    ihm_menu(g_uart);

    printf("\n\n");
    LOG_INFO("Interface menu finished");

    // Clean up 
    cleanup_and_exit(0);
}