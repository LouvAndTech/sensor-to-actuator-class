#include "button.h"
#include "gpio.h"
#include "stdio.h"
#include "stdlib.h"
#include "main.h"
#include "usart.h"


uint8_t button_pressed = 0;
uint32_t last_called_time = 0;
/**
 * @brief Callback pour gérer l'appui sur le bouton
 */
void BUTTON_Callback(void)
{
    if (HAL_GetTick() - last_called_time < 200)
    {
        return; // Ignore the callback if it was called too soon
    }
    last_called_time = HAL_GetTick();
    button_pressed = 1;
    HAL_UART_Transmit(&huart2, (uint8_t *)"Button Pressed!\r\n", 17, HAL_MAX_DELAY);
}

/**
 * @brief Initialise le bouton et les LEDs
 */
void BUTTON_Init(void)
{
    // Initialiser le bouton avec un callback
    GPIO_RegisterGPIO_EXTICallback(BUTTON_Callback, Bouton_1_Pin);
}

/**
 * @brief Récupère l'état actuel
 */
uint8_t BUTTON_GetState(void)
{
    return button_pressed;
}

/**
 * @brief Réinitialise l'état du bouton
 */
void BUTTON_reset(void)
{
    button_pressed = 0;
}