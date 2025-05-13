#include "uart.h"

// Libraries
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <mqueue.h>
#include <string.h>

#include "../../libs/arduino-serial-lib/arduino-serial-lib.h"


#define MQ_MSG_MAX (10)
#define MQ_NAME_RX "/uart_queue_rx"
#define MQ_NAME_TX "/uart_queue_tx"

// Static variables
static int uart_count = 0;

// Types 
struct UART_t{
    int openned;
    int enabled;
    int serial_port;
    pthread_t read_thread;
    UART_param param;
    mqd_t message_queue_rx;
    mqd_t message_queue_tx;
};

// Private functions prototypes
static int open_uart(UART* uart, const char* device, int baud);
static int close_uart(UART* uart);
static void run_uart(UART* uart);

// Public functions 
extern UART* uart_new(UART_param* param){
    UART* uart = malloc(sizeof(UART));
    if (uart == NULL) {
        perror("Error allocating memory for UART");
        return NULL;
    }

    uart->param = *param;

    // Init the message queue
	struct mq_attr attr;

	attr.mq_maxmsg = MQ_MSG_MAX;
	attr.mq_msgsize = uart->param.message_max_length;

    char rx_queue_name[256];
    snprintf(rx_queue_name, sizeof(rx_queue_name), "%s_%d", MQ_NAME_RX, uart_count);
	if ((uart->message_queue_rx = mq_open(rx_queue_name, O_RDWR | O_CREAT | O_NONBLOCK, 0644, &attr)) == -1) {
		perror("Error creating message queue rx");
        free(uart);
        return NULL;
	}

    char tx_queue_name[256];
    snprintf(tx_queue_name, sizeof(tx_queue_name), "%s_%d", MQ_NAME_TX, uart_count);
    if ((uart->message_queue_tx = mq_open(tx_queue_name, O_RDWR | O_CREAT | O_NONBLOCK, 0644, &attr)) == -1) {
        perror("Error creating message queue tx");
        mq_close(uart->message_queue_rx);
        free(uart);
        return NULL;
	}

    
    // Open the UART device
    if (open_uart(uart, uart->param.device, uart->param.baud) == -1) {
        fprintf(stderr, "Error opening UART device: %s\n", uart->param.device);
        mq_close(uart->message_queue_rx);
        mq_close(uart->message_queue_tx);
        free(uart);
        return NULL;
    }

    uart->enabled = 0;
    uart_count++; 
    return uart;
}

extern int uart_free(UART* uart){
    if (uart == NULL || uart->enabled) {
        fprintf(stderr, "Error: UART is still enabled or NULL\n");
        return -1;
    }
    if (close_uart(uart) == -1) {
        fprintf(stderr, "Error closing UART device\n");
        return -1;
    }
    if (mq_close(uart->message_queue_rx) == -1) {
        perror("Error closing message queue rx");
        return -1;
    }
    if (mq_close(uart->message_queue_tx) == -1) {
        perror("Error closing message queue tx");
        return -1;
    }
    free(uart);
    return 0;
}

extern int uart_start(UART* uart){
    if (uart == NULL || !uart->openned || uart->enabled) {
        fprintf(stderr, "Error: UART is not opened or already enabled\n");
        return -1;
    }

    uart->enabled = 1;

    int thread_result = pthread_create(&uart->read_thread, NULL, (void* (*)(void*))run_uart, uart);
    if (thread_result != 0) {
        fprintf(stderr, "Error creating thread: %s\n", strerror(thread_result));
        return -1;
    }
    thread_result = pthread_detach(uart->read_thread);
    if (thread_result != 0) {
        fprintf(stderr, "Error detaching thread: %s\n", strerror(thread_result));
        return -1;
    }
    
    return 0;
}


extern int uart_stop(UART* uart){
    if (uart == NULL || !uart->enabled) {
        fprintf(stderr, "Error: UART is not enabled or NULL\n");
        return -1;
    }

    uart->enabled = 0;

    pthread_cancel(uart->read_thread);
    pthread_join(uart->read_thread, NULL);

    return 0;
}

extern int uart_is_enabled(UART* uart){
    if (uart == NULL) {
        fprintf(stderr, "Error: UART is NULL\n");
        return -1;
    }
    return uart->enabled;
}

extern int uart_get_message(UART* uart, char* buffer){
    if (uart == NULL || buffer == NULL) {
        fprintf(stderr, "Error: UART or buffer is NULL\n");
        return -1;
    }

    ssize_t bytes_read = mq_receive(uart->message_queue_rx, buffer, uart->param.message_max_length, NULL);
    if (bytes_read < 0) {
        if (errno == EAGAIN) {
            // Timeout
            //fprintf(stderr, "No message available in queue rx\n");
            return 0; // No message received
        } else {
            perror("Error receiving message from queue rx");
            return -1;
        }
    }
    //fprintf(stdout,"buffer : %s\n", buffer);


    buffer[bytes_read] = '\0'; // Null-terminate the string
    return bytes_read;
}

extern int uart_send_message(UART* uart, const char* message, size_t length){
    if (uart == NULL || message == NULL) {
        fprintf(stderr, "Error: UART or message is NULL\n");
        return -1;
    }
    if (length > uart->param.message_max_length) {
        fprintf(stderr, "Error: Message length exceeds maximum length\n");
        return -1;
    }

    if (mq_send(uart->message_queue_tx, message, length, 0) == -1) {
        perror("Error sending message to queue tx");
        return -1;
    }

    return 0;
}

extern int uart_get_buffer_length(UART* uart){
    if (uart == NULL) {
        fprintf(stderr, "Error: UART is NULL\n");
        return -1;
    }

    return uart->param.message_max_length;
}


//Private functions
static int open_uart(UART* uart, const char* device, int baud){
    if (uart == NULL || device == NULL || baud <= 0) {
        fprintf(stderr, "Error: Invalid parameters\n");
        return -1;
    }
    
    uart->serial_port = serialport_init(device, baud);
    if (uart->serial_port == -1) {
        fprintf(stderr, "Error opening serial port: %s\n", strerror(errno));
        return -1;
    }
    
    if(serialport_flush(uart->serial_port) == -1) {
        fprintf(stderr, "Error flushing serial port: %s\n", strerror(errno));
        serialport_close(uart->serial_port);
        return -1;
    }

    uart->openned = 1;
    return 0;
}

static int close_uart(UART* uart){
    if (uart == NULL) {
        fprintf(stderr, "Error: UART is NULL\n");
        return -1;
    }
    
    if (serialport_close(uart->serial_port) == -1) {
        fprintf(stderr, "Error closing serial port: %s\n", strerror(errno));
        return -1;
    }

    uart->openned = 0;
    return 0;
}

static void run_uart(UART* uart){
    if (uart == NULL || !uart->openned) {
        fprintf(stderr, "Error: UART is not opened or NULL\n");
        return;
    }
    
    char buffer[uart->param.message_max_length];
    while (uart->enabled) {

        // Read from the UART device
        int err = serialport_read_until(uart->serial_port, buffer, uart->param.end_line, sizeof(buffer), uart->param.timeout);
        if (err == -1) {
            fprintf(stderr, "Error reading from UART: %s\n", strerror(errno));
        } else if (err == -2) {
            // Timeout occurred
            //fprintf(stderr, "Timeout occurred while reading from UART\n");
        } else if (err == 0) {
            
            //printf("Received: %s\n", buffer);
            if (mq_send(uart->message_queue_rx, buffer, strlen(buffer)+1, 0) == -1) {
                if (errno == EAGAIN) {
                    // Queue is full
                    //read one message to free space then send the new one
                    char temp_buffer[uart->param.message_max_length];
                    ssize_t mq_received = mq_receive(uart->message_queue_rx, temp_buffer, sizeof(temp_buffer), NULL);
                    if (mq_received >= 0) {
                        // Successfully read a message from the queue
                        // Now send the new message
                        if (mq_send(uart->message_queue_rx, buffer, strlen(buffer)+1, 0) == -1) {
                            perror("Error sending message to queue rx");
                        }
                    } else {
                        perror("Error receiving message from queue rx");
                    }
                } else {
                    perror("Error sending message to queue rx");
                }
            }
        }

        // Write to the UART device
        ssize_t mq_received = mq_receive(uart->message_queue_tx, buffer, sizeof(buffer), NULL);
        if (mq_received >= 0) {
            //fprintf(stdout,"\nsending: %s\n", buffer);
            if (serialport_write(uart->serial_port, buffer) == -1) {
                fprintf(stderr, "Error writing to UART: %s\n", strerror(errno));
            }
        } else {
            if (errno == EAGAIN) {
                // Timeout
                //fprintf(stderr, "No message available in queue tx\n");
            } else {
                perror("Error receiving message from queue tx");
            }
        }
    }
}