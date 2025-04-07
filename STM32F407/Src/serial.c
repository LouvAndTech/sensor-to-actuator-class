#include "serial.h"
#include "usart.h"
#include "string.h"

void SERIAL_Init(void)
{
    //Empty since it's supposed started in main.c
}

void SERIAL_SendString(const char *str)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}

void SERIAL_receiveString(char *buffer, uint16_t size)
{
    HAL_UART_Receive(&huart2, (uint8_t *)buffer, size, HAL_MAX_DELAY);
    buffer[size] = '\0'; // Null-terminate the string
}
