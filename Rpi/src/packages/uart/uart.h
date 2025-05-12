#ifndef UART_H
#define UART_H

#include <stdlib.h>
#include <stdio.h>

/**
 * @struct UART_t
 * @brief UART structure to hold the UART device information.
 */
typedef struct UART_t UART;

/**
 * @struct UART_param
 * @brief UART parameters structure to hold the device name, baud rate, and end line character.
 */
typedef struct {
    const char* device;     ///< The device name (e.g., "/dev/ttyS0").
    int baud;               ///< The baud rate for the UART communication.
    char end_line;          ///< The end line character for the UART communication.
    int message_max_length; ///< The maximum length of the message to be received.
    int timeout;           ///< The timeout for the UART communication.
}UART_param;

/**
 * @brief Creates a new UART instance.
 * 
 * @param device The device name (e.g., "/dev/ttyS0").
 * @param baud The baud rate for the UART communication.
 * 
 * @return A pointer to the new UART instance, or NULL on failure.
 */
UART* uart_new(UART_param* param);

/**
 * @brief Frees the UART instance.
 * 
 * @param uart The UART instance to free.
 * 
 * @return 0 on success, -1 on failure.
 */
int uart_free(UART* uart);

/**
 * @brief Open the UART device and start reading asynchronously.
 * 
 * @note You need to call this function before using the UART instance for either reading or writing.
 * 
 * @param uart The UART instance to start.
 * 
 * @return 0 on success, -1 on failure.
 */
int uart_start(UART* uart);
/**
 * @brief Stop the UART device and stop reading asynchronously.
 * 
 * @note You need to call this function before freeing the UART instance.
 * 
 * @param uart The UART instance to stop.
 * 
 * @return 0 on success, -1 on failure.
 */
int uart_stop(UART* uart);

/**
 * @brief Check if the UART device is enabled.
 * 
 * @param uart The UART instance to check.
 * @return 1 if enabled, 0 otherwise.
 */
int uart_is_enabled(UART* uart);

/**
 * @brief Get a message from the list of received messages.
 * 
 * @note This function work with a fifo queue, so it will return the first message received.
 * 
 * @param uart The UART instance to get the message from.
 * @param buffer The buffer to store the message. It is your responsibility to allocate enough memory for the buffer. You can call uart_get_buffer_length(uart) to get the minimum length of the buffer.
 * 
 * @return The length of the message, 0 if there is no message, or -1 on failure.
 */
int uart_get_message(UART* uart, char* buffer);
/**
 * @brief Send a message to the UART device.
 * 
 * @param uart The UART instance to send the message to.
 * @param message The message to send.
 * 
 * @return 0 on success, -1 on failure.
 */
int uart_send_message(UART* uart, const char* message, size_t length);

/**
 * @brief Get the length of the buffer in the UART instance.
 * 
 * @param uart The UART instance to get the buffer length from.
 * 
 * @return The length of the buffer, or -1 on failure.
 */
int uart_get_buffer_length(UART* uart);


#endif // UART_H