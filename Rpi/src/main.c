#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "./packages/pwm/pwm.h"
#include "./packages/uart/uart.h"


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

    // Main loop to receive messages and send commands
    int percentage = 0;

    printf("Starting main loop...\n");
    while (1) {
        printf("Waiting for messages...\n");
        char message[MAX_MESSAGE_LENGTH];
        int bytes_read = uart_get_message(uart, message);
        if (bytes_read == -1) {
            fprintf(stderr, "Error receiving message from UART\n");
        } else if (bytes_read == 0) {
            // Timeout
            //printf("No message received.\n");
        } else {
            printf("Received message: %s\n", message);
        }

        
        // Format the percentage as a 3-character string
        char command[16];
        snprintf(command, sizeof(command), "servo:%03d\n", percentage);
        printf("Sending command: %s", command);

        // Send the command
        if(uart_send_message(uart, command, strlen(command))) {
            fprintf(stderr, "Error sending command to UART\n");
        }

        // Increment percentage and reset if it exceeds 100
        percentage = (percentage + 1) % 101;

        sleep(1); // Sleep for 1 second before sending the next command
    }





    /*
    printf("Starting API PWM example...\n");

    // Initialize the PWM signal
    PWM *pwm = pwm_new(CHIP_NAME, LINE_NUMBER);
    if (!pwm) {
        fprintf(stderr, "Failed to initialize PWM\n");
        return 1;
    }

    // Set the duty cycle to 10.5%
    pwm_set_duty_cycle(pwm, 7.5);
    pwm_start(pwm);
    printf("PWM signal started with 7.5%% duty cycle.\n");


    pwm_set_duty_cycle(pwm, 5); // 0°
    printf("Servo to 0°.\n");
    sleep(10); 
    pwm_set_duty_cycle(pwm, 7.5); // 90°
    printf("Servo to 90°.\n");
    sleep(10); 
    pwm_set_duty_cycle(pwm, 10); // 180°
    printf("Servo to 180°.\n");
    sleep(10);

    // Stop the PWM signal
    pwm_stop(pwm);
    printf("PWM signal stopped.\n");

    // Free the PWM resources
    pwm_free(pwm);
    printf("PWM resources cleaned up.\n");
    */


    return 0;
}