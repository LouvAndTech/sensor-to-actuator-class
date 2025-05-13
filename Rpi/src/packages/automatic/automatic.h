#ifndef AUTOMATIC_H
#define AUTOMATIC_H

#include "../uart/uart.h"
#include "../pwm/pwm.h"

/**
 * @brief Initializes the automatic mode.
 * 
 * @param uart Pointer to the UART device.
 * @param pwm Pointer to the PWM device.
 */
void automatic_init(UART* uart, PWM* pwm);

/**
 * @brief Starts the automatic mode in a thread that reads from the UART device and writes to the PWM device the value received before writting the value to the UART device.
 * 
 * @param uart Pointer to the UART device.
 * @param pwm Pointer to the PWM device.
 */
void automatic_start(void);

/**
 * @brief Stops the automatic mode.
 * 
 * @note This function should be called to stop the automatic mode before freeing the UART and PWM devices.
 */
void automatic_stop(void);

#endif // AUTOMATIC_H