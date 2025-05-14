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
#include "../logger/logger.h"
#include "../../utils.h"


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
        LOG_PERROR("Error allocating memory for UART");
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
        LOG_PERROR("Error creating message queue rx");
        free(uart);
        return NULL;
	}

    char tx_queue_name[256];
    snprintf(tx_queue_name, sizeof(tx_queue_name), "%s_%d", MQ_NAME_TX, uart_count);
    if ((uart->message_queue_tx = mq_open(tx_queue_name, O_RDWR | O_CREAT | O_NONBLOCK, 0644, &attr)) == -1) {
        LOG_PERROR("Error creating message queue tx");
        mq_close(uart->message_queue_rx);
        free(uart);
        return NULL;
	}

    
    // Open the UART device
    if (open_uart(uart, uart->param.device, uart->param.baud) == -1) {
        LOG_ERROR("Error opening UART device: %s", uart->param.device);
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
        LOG_ERROR("Error: UART is still enabled or NULL");
        return -1;
    }
    if (close_uart(uart) == -1) {
        LOG_ERROR("Error closing UART device");
        return -1;
    }
    if (mq_close(uart->message_queue_rx) == -1) {
        LOG_PERROR("Error closing message queue rx");
        return -1;
    }
    if (mq_close(uart->message_queue_tx) == -1) {
        LOG_PERROR("Error closing message queue tx");
        return -1;
    }
    free(uart);
    return 0;
}

extern int uart_start(UART* uart){
    if (uart == NULL || !uart->openned || uart->enabled) {
        LOG_ERROR("Error: UART is not opened or already enabled");
        return -1;
    }

    uart->enabled = 1;

    int thread_result = pthread_create(&uart->read_thread, NULL, (void* (*)(void*))run_uart, uart);
    if (thread_result != 0) {
        LOG_ERROR("Error creating thread: %s", strerror(thread_result));
        return -1;
    }
    thread_result = pthread_detach(uart->read_thread);
    if (thread_result != 0) {
        LOG_ERROR("Error detaching thread: %s", strerror(thread_result));
        return -1;
    }
    
    return 0;
}


extern int uart_stop(UART* uart){
    if (uart == NULL || !uart->enabled) {
        LOG_ERROR("Error: UART is not enabled or NULL");
        return -1;
    }

    uart->enabled = 0;

    pthread_cancel(uart->read_thread);
    pthread_join(uart->read_thread, NULL);

    return 0;
}

extern int uart_is_enabled(UART* uart){
    if (uart == NULL) {
        LOG_ERROR("Error: UART is NULL");
        return -1;
    }
    return uart->enabled;
}

extern int uart_get_message(UART* uart, char* buffer){
    if (uart == NULL || buffer == NULL) {
        LOG_ERROR("Error: UART or buffer is NULL");
        return -1;
    }

    ssize_t bytes_read = mq_receive(uart->message_queue_rx, buffer, uart->param.message_max_length, NULL);
    if (bytes_read < 0) {
        if (errno == EAGAIN) {
            // Timeout
            //fprintf(stderr, "No message available in queue rx\n");
            return 0; // No message received
        } else {
            LOG_PERROR("Error receiving message from queue rx");
            return -1;
        }
    }
    //fprintf(stdout,"buffer : %s\n", buffer);


    buffer[bytes_read] = '\0'; // Null-terminate the string
    return bytes_read;
}

extern int uart_send_message(UART* uart, const char* message, size_t length){
    if (uart == NULL || message == NULL) {
        LOG_ERROR("Error: UART or message is NULL");
        return -1;
    }
    if (length > uart->param.message_max_length) {
        LOG_ERROR("Error: Message length exceeds maximum length");
        return -1;
    }

    if (mq_send(uart->message_queue_tx, message, length, 0) == -1) {
        LOG_PERROR("Error sending message to queue tx");
        return -1;
    }

    return 0;
}

extern int uart_get_buffer_length(UART* uart){
    if (uart == NULL) {
        LOG_ERROR("Error: UART is NULL");
        return -1;
    }

    return uart->param.message_max_length;
}


//Private functions
static int open_uart(UART* uart, const char* device, int baud){
    if (uart == NULL || device == NULL || baud <= 0) {
        LOG_ERROR("Error: Invalid parameters");
        return -1;
    }
    
    uart->serial_port = serialport_init(device, baud);
    if (uart->serial_port == -1) {
        LOG_PERROR("Error opening serial port");
        return -1;
    }
    
    if(serialport_flush(uart->serial_port) == -1) {
        LOG_PERROR("Error flushing serial port");
        serialport_close(uart->serial_port);
        return -1;
    }

    uart->openned = 1;
    return 0;
}

static int close_uart(UART* uart){
    if (uart == NULL) {
        LOG_ERROR("Error: UART is NULL");
        return -1;
    }
    
    if (serialport_close(uart->serial_port) == -1) {
        LOG_PERROR("Error closing serial port");
        return -1;
    }

    uart->openned = 0;
    return 0;
}

static void run_uart(UART* uart){
    if (uart == NULL || !uart->openned) {
        LOG_ERROR("Error: UART is not opened or NULL");
        return;
    }
    
    char buffer_rx[uart->param.message_max_length];
    char buffer_tx[uart->param.message_max_length];
    while (uart->enabled) {

        // Read from the UART device
        int err = serialport_read_until(uart->serial_port, buffer_rx, uart->param.end_line, sizeof(buffer_rx), uart->param.timeout);
        if (err == -1) {
            LOG_PERROR("Error reading from UART");
        } else if (err == -2) {
            // Timeout occurred
            //fprintf(stderr, "Timeout occurred while reading from UART\n");
        } else if (err == 0) {
            
            //printf("Received: %s\n", buffer_rx);
            if (mq_send(uart->message_queue_rx, buffer_rx, strlen(buffer_rx), 0) == -1) {
                if (errno == EAGAIN) {
                    // Queue is full
                    //read one message to free space then send the new one
                    char temp_buffer[uart->param.message_max_length];
                    ssize_t mq_received = mq_receive(uart->message_queue_rx, temp_buffer, sizeof(temp_buffer), NULL);
                    if (mq_received >= 0) {
                        // Successfully read a message from the queue

                        CLEAN_STRING(temp_buffer);
                        logger_log(INFO, "Message dropped from queue rx : %s", temp_buffer);
                        
                        // Now send the new message
                        if (mq_send(uart->message_queue_rx, buffer_rx, strlen(buffer_rx), 0) == -1) {
                            LOG_PERROR("Error sending message to queue rx");
                        }
                    } else {
                        LOG_PERROR("Error receiving message from queue rx");
                    }
                } else {
                    LOG_PERROR("Error sending message to queue rx");
                }
            }
        }

        // Write to the UART device
        ssize_t mq_received = mq_receive(uart->message_queue_tx, buffer_tx, sizeof(buffer_tx), NULL);
        if (mq_received >= 0) {
            buffer_tx[mq_received] = '\0'; // Null-terminate the string
            //fprintf(stdout,"\nsending: %s\n", buffer_tx);
            if (serialport_write(uart->serial_port, buffer_tx) == -1) {
                LOG_PERROR("Error writing to UART");
            }
        } else {
            if (errno == EAGAIN) {
                // Timeout
                //fprintf(stderr, "No message available in queue tx\n");
            } else {
                LOG_PERROR("Error receiving message from queue tx");
            }
        }
    }
}