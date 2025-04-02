/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
  ******************************************************************************
  */

#include "main.h"
#include "leds.h"
#include "tim.h"
#include "gpio.h"

/* Prototypes */
void callback_chenillar(void);

/* Public functions */
/**
 * @brief 
 * 
 */
void LEDS_Init(void)
{
    RegisterTimerCallback(callback_chenillar, &htim6);
    HAL_TIM_Base_Start_IT(&htim6);
}

/**
 * @brief callback function for the chenillar effect called on timer interrupt
 * 
 */
void callback_chenillar(void){
    static uint8_t divider = 0;
    if (divider == 0)
    {
        static uint8_t current_led = 0;
        HAL_GPIO_WritePin(GPIOD, LD3_Pin | LD4_Pin | LD5_Pin | LD6_Pin, GPIO_PIN_RESET);
        switch(current_led)
        {
        case 0:
            HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_SET);
            break;
        case 1:
            HAL_GPIO_WritePin(GPIOD, LD4_Pin, GPIO_PIN_SET);
            break;
        case 2:
            HAL_GPIO_WritePin(GPIOD, LD6_Pin, GPIO_PIN_SET);
            break;
        case 3:
            HAL_GPIO_WritePin(GPIOD, LD5_Pin, GPIO_PIN_SET);
            break;
        }
        current_led = (current_led + 1) % 4;
    }
    divider = (divider + 1) % 1000;
}