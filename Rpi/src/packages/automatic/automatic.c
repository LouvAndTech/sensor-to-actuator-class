#include "automatic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "../../utils.h"

static int active= 0;
static pthread_t thread_id;
static UART* uart;
static PWM* pwm;

// Private function prototypes
static void run(void);

// Public functions

extern void automatic_init(UART* uart_device, PWM* pwm_device){
    if (uart_device == NULL || pwm_device == NULL) {
        LOG_ERROR("Error: UART or PWM is NULL");
        return;
    }
    
    uart = uart_device;
    pwm = pwm_device;

    return;
}

extern void automatic_start(void){
    if (uart == NULL || pwm == NULL) {
        LOG_ERROR("Error: UART or PWM is NULL");
        return;
    }
    
    if (active) {
        LOG_ERROR("Error: Automatic mode is already running");
        return;
    }
    
    active = 1;

    // start the pwm
    pwm_set_duty_cycle(pwm, 0);
    pwm_start(pwm);

    int thread_result = pthread_create(&thread_id, NULL, (void* (*)(void*))run, NULL);
    if (thread_result != 0) {
        LOG_PERROR("Error creating thread");
        active = 0;
        return;
    }
    thread_result = pthread_detach(thread_id);
    if (thread_result != 0) {
        LOG_PERROR("Error detaching thread");
        active = 0;
        return;
    }
    
    pthread_detach(thread_id);
}

extern void automatic_stop(void){
    if (!active) {
        LOG_ERROR("Error: Automatic mode is not running");
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
        LOG_ERROR("Error: UART or PWM is NULL");
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
                    LOG_WARN("Received value out of range (0-100): %d", value);
                    continue;
                }
                
                // Set the PWM value
                pwm_set_duty_cycle(pwm, value);
                LOG_INFO("Set PWM value to %d", value);
                // Send the value back to UART
                char response[50];
                snprintf(response, sizeof(response), "servo:%03d\n", value);
                uart_send_message(uart, response, strlen(response));

                CLEAN_STRING(response);
                LOG_INFO("Sent response to UART: %s", response);
            } else {
                
                CLEAN_STRING(buffer);
                LOG_WARN("Invalid message format received from UART: %s", buffer);
            }
        } else if (bytes_received == -1) {
            LOG_ERROR("Error receiving message from UART");
        }
        usleep(100000); // Sleep for 100ms
    }
}