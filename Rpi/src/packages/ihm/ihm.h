#ifndef IHM_H
#define IHM_H

#include "../uart/uart.h"

/**
 * @brief Displays the terminal interface menu and handles user input.
 * 
 * @param uart The UART instance for communication.
 */
void ihm_menu(UART* uart);

#endif // IHM_H