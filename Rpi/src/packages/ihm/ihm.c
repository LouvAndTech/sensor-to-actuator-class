#include "ihm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../uart/uart.h"

#define MAX_MESSAGE_LENGTH 100

typedef enum {
    MENU=0,
    SHOW_LAST_VALUE,
    SET_NEW_VALUE,
    EXIT
} ihm_status_t;

void ihm_menu(UART* uart) {
    int choice;
    char last_received_message[MAX_MESSAGE_LENGTH] = {0};

    while (1) {
        printf("\n--- Terminal Interface Menu ---\n");
        printf("1. Show the last value received from UART\n");
        printf("2. Set a new value (0-100)\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case SHOW_LAST_VALUE: {
                int max_length = uart_get_buffer_length(uart);
                if (max_length <= 0) {
                    fprintf(stderr, "Error: Invalid buffer length\n");
                    break;
                }
                char buffer[max_length];
                memset(buffer, 0, sizeof(buffer));
                if (uart_get_message(uart, buffer) > 0) {
                    strncpy(last_received_message, buffer, MAX_MESSAGE_LENGTH - 1);
                    printf("\nLast received value: %s\n", last_received_message);
                } else {
                    printf("No value received or error occurred.\n");
                }
                break;
            }

            case SET_NEW_VALUE: {
                int new_value;
                printf("Enter a value between 0 and 100: ");
                scanf("%d", &new_value);

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

            case EXIT:
                printf("Exiting the terminal interface.\n");
                return;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}