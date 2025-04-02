/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : SonicSensor_Echo_Pin */
  GPIO_InitStruct.Pin = SonicSensor_Echo_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(SonicSensor_Echo_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MEMS_INT2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 2 */
typedef struct {
  uint16_t GPIO_Pin;
  GPIO_EXTICallback callback;
} GPIO_EXTICallbackStruct;

static GPIO_EXTICallbackStruct gpio_callbacks[MAX_GPIO_EXTI_CALLBACKS];
static int gpio_callback_count = 0;

/**
 * @brief Register a callback for a specific GPIO EXTI pin.
 * 
 * @param callback 
 * @param GPIO_Pin 
 */
void GPIO_RegisterGPIO_EXTICallback(GPIO_EXTICallback callback, uint16_t GPIO_Pin)
{
  if (gpio_callback_count < MAX_GPIO_EXTI_CALLBACKS)
  {
    gpio_callbacks[gpio_callback_count].GPIO_Pin = GPIO_Pin;
    gpio_callbacks[gpio_callback_count].callback = callback;
    gpio_callback_count++;
  }
  else
  {
    // Handle error: too many callbacks registered
    // For now, we will just ignore the new callback
  }
}

/**
 * @brief Unregister a specific callback for a specific GPIO EXTI pin.
 * 
 * @param callback 
 * @param GPIO_Pin 
 */
void GPIO_UnregisterGPIO_EXTICallback(GPIO_EXTICallback callback, uint16_t GPIO_Pin)
{
  for (int i = 0; i < gpio_callback_count; i++)
  {
    if (gpio_callbacks[i].GPIO_Pin == GPIO_Pin && gpio_callbacks[i].callback == callback)
    {
      // Shift the remaining callbacks down
      for (int j = i; j < gpio_callback_count - 1; j++)
      {
        gpio_callbacks[j] = gpio_callbacks[j + 1];
      }
      gpio_callback_count--;
      break;
    }
  }
}

/**
 * @brief Handle all Callbacks for the GPIO EXTI interrupts.
 * 
 * @param GPIO_Pin 
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  for (int i = 0; i < gpio_callback_count; i++)
  {
    if (gpio_callbacks[i].GPIO_Pin == GPIO_Pin)
    {
      gpio_callbacks[i].callback();
      break;
    }
  }
}

/* USER CODE END 2 */
