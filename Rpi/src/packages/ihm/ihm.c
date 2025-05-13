#include "ihm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../uart/uart.h"
#include "../automatic/automatic.h"


#define MAX_MESSAGE_LENGTH 100

typedef enum {
    MENU=0,
    SHOW_LAST_VALUE,
    SET_NEW_VALUE,
    AUTOMATIC_MODE,
    EXIT
} ihm_status_t;

static int check_intput_int(int scanf_result) {
    if (scanf_result != 1) {
        fprintf(stderr, "Invalid input. Please enter a number.\n");
        while (getchar() != '\n'); // Clear the input buffer
        return -1;
    }
    return 0;
}

void ihm_menu(UART* uart) {
    int choice;
    char last_received_message[MAX_MESSAGE_LENGTH] = {0};

    while (1) {
        printf("\n--- Terminal Interface Menu ---\n");
        printf("1. Show the last value received from UART\n");
        printf("2. Set a new value (0-100)\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        if (check_intput_int(scanf("%d", &choice)) != 0) {
            continue; // Invalid input, prompt again
        }

        switch (choice) {
            case SHOW_LAST_VALUE: {
                int max_length = uart_get_buffer_length(uart);
                if (max_length <= 0) {
                    fprintf(stderr, "Error: Invalid buffer length\n");
                    break;
                }
                char buffer[max_length];
                memset(buffer, 0, sizeof(buffer));
                int bytes_received = uart_get_message(uart, buffer);
                if ( bytes_received > 0) {
                    strncpy(last_received_message, buffer, MAX_MESSAGE_LENGTH - 1);
                    printf("\nLast received value: %s\n", last_received_message);
                } else {
                    if (bytes_received == -1) {
                        fprintf(stderr, "Error receiving message from UART\n");
                    } else if (bytes_received == 0) {
                        printf("No new message received.\n");
                    }
                }
                break;
            }

            case SET_NEW_VALUE: {
                int new_value;
                printf("Enter a value between 0 and 100: ");
                if (check_intput_int(scanf("%d", &new_value)) != 0) {
                    continue; // Invalid input, prompt again
                }

                if (new_value < 0 || new_value > 100) {
                    printf("Invalid value. Please enter a number between 0 and 100.\n");
                } else {
                    char command[16];
                    snprintf(command, sizeof(command), "servo:%03d\n", new_value);

                    if (uart_send_message(uart, command, strlen(command)) == 0) {
                        printf("\nCommand sent successfully: %s", command);
                    } else {
                        fprintf(stderr, "Error sending command to UART\n");
                    }
                }
                break;
            }
            case AUTOMATIC_MODE:
                printf("Starting automatic mode...\n");
                automatic_start(); // Start the automatic mode
                printf("Automatic mode started.\n");
                printf("Press Enter to stop automatic mode...\n");
                getchar(); // Wait for user input
                automatic_stop();
                printf("Automatic mode stopped.\n");
                break;

            case EXIT:
                printf("Exiting the terminal interface.\n");
                return;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}