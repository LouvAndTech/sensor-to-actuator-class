#include "automatic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

static int active= 0;
static pthread_t thread_id;
static UART* uart;
static PWM* pwm;

// Private function prototypes
static void run(void);

// Public functions

extern void automatic_init(UART* uart_device, PWM* pwm_device){
    if (uart_device == NULL || pwm_device == NULL) {
        fprintf(stderr, "Error: UART or PWM is NULL\n");
        return;
    }
    
    uart = uart_device;
    pwm = pwm_device;

    return;
}

extern void automatic_start(void){
    if (uart == NULL || pwm == NULL) {
        fprintf(stderr, "Error: UART or PWM is NULL\n");
        return;
    }
    
    if (active) {
        fprintf(stderr, "Error: Automatic mode is already running\n");
        return;
    }
    
    active = 1;

    // start the pwm
    pwm_set_duty_cycle(pwm, 0);
    pwm_start(pwm);

    int thread_result = pthread_create(&thread_id, NULL, (void* (*)(void*))run, NULL);
    if (thread_result != 0) {
        fprintf(stderr, "Error creating thread: %s\n", strerror(thread_result));
        active = 0;
        return;
    }
    thread_result = pthread_detach(thread_id);
    if (thread_result != 0) {
        fprintf(stderr, "Error detaching thread: %s\n", strerror(thread_result));
        active = 0;
        return;
    }
    
    pthread_detach(thread_id);
}

extern void automatic_stop(void){
    if (!active) {
        fprintf(stderr, "Error: Automatic mode is not running\n");
        return;
    }
    
    active = 0;
    
    // Wait for the thread to finish
    pthread_cancel(thread_id);
    pthread_join(thread_id, NULL);

    // Stop the PWM
    pwm_stop(pwm);
    pwm_set_duty_cycle(pwm, 0);
}


// private function
static void run(void){
    if (uart == NULL || pwm == NULL) {
        fprintf(stderr, "Error: UART or PWM is NULL\n");
        return;
    }
    
    char buffer[uart_get_buffer_length(uart)];
    int bytes_received = 0;
    
    while (active) {
        // Read from UART
        bytes_received = uart_get_message(uart, buffer);
        if (bytes_received > 0) {
            // Process the received message in the format "sensor:069\n"
            int sensor = -1;
            if (sscanf(buffer, "sensor:%d", &sensor) == 1) {

                // Compute the percentage value from the received value
                int value = (sensor * 100) / 60; // Assuming the sensor value is between 0 and 60

                // Set the PWM value
                if (value < 0 || value > 100) {
                    fprintf(stderr, "Error: Invalid value received from UART\n");
                    continue;
                }
                
                // Set the PWM value
                pwm_set_duty_cycle(pwm, value);
                printf("Set PWM value to %d\n", value);
                // Send the value back to UART
                char response[50];
                snprintf(response, sizeof(response), "servo:%03d\n", value);
                uart_send_message(uart, response, strlen(response));
                printf("Sent response to UART: %s", response);
            } else {
                fprintf(stderr, "Error: Invalid message format received from UART\n");
            }
        } else if (bytes_received == -1) {
            fprintf(stderr, "Error receiving message from UART\n");
        }
        usleep(100000); // Sleep for 100ms
    }
}