/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdlib.h"
#include "ultra_sonic.h"
#include "leds.h"
#include "servo.h"
#include "serial.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE BEGIN PV */
uint8_t rxData[10];
volatile uint8_t rxFlag = 0;  // Indique la réception complète d'une trame UART
char buffer[10];
#define CMD_BUF_SIZE 32
uint8_t  rxChar;
char     cmdBuf[CMD_BUF_SIZE];
uint8_t  cmdLen = 0;
float distance;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Global variables to store the distance

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM6_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  // Test the UART
  HAL_UART_Transmit(&huart2, (uint8_t *)"Program Start :\r\n", 13, HAL_MAX_DELAY);
  HAL_UART_Receive_IT(&huart2, &rxChar, 1);

  //
  SERIAL_Init();

  HAL_UART_Receive_IT(&huart3, &rxChar, 1);

  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

/* USER CODE BEGIN 2 */

// Initialisation des composants
LEDS_Init();
ULTRA_SONIC_Init();
SERVO_Init();
SERIAL_Init();
HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

// Timer pour envoyer toutes les 250 ms
uint32_t lastTick = HAL_GetTick();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
while (1)
{

    // Envoi périodique de la distance (250 ms)
    if (HAL_GetTick() - lastTick >= 250)
    {
        lastTick = HAL_GetTick();

        distance = ULTRA_SONIC_GetDistance();
        if (distance > 100) distance = 100;

        // Envoie distance sur UART
        sprintf(buffer, "sensor:%03d\n", (int)distance);
        SERIAL_SendString(buffer);
        HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);

        LEDS_SetBlue(); // Indique l'activité
    }

    // Traitement de la commande reçue (non bloquant)
    if (rxFlag)
    {
        rxFlag = 0;

        // Parsing « servo:XX »
        if (strncmp(cmdBuf, "servo:", 6) == 0)
        {
            int pct = atoi(cmdBuf + 6);
            if (pct >= 0 && pct <= 100)
            {
                SERVO_set_servo_percentage(pct);
                HAL_UART_Transmit(&huart3, (uint8_t *)"Servo moved\r\n", 13, HAL_MAX_DELAY);
            }
            else
            {
                HAL_UART_Transmit(&huart3, (uint8_t *)"Pct invalide (0-100)\r\n", 21, HAL_MAX_DELAY);
            }
        }
        else
        {
            HAL_UART_Transmit(&huart3, (uint8_t *)"Cmd invalide\r\n", 14, HAL_MAX_DELAY);
        }
    }
}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
}
  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {

        // Afficher caractère par caractère
        HAL_UART_Transmit(&huart2, &rxChar, 1, HAL_MAX_DELAY);
        // Stocke dans le tampon
        if (cmdLen < CMD_BUF_SIZE-1)
            cmdBuf[cmdLen++] = rxChar;

        // Si fin de ligne, on lève le flag
        if (rxChar == '\n' || rxChar == '\r')
        {
            cmdBuf[cmdLen] = '\0';
            rxFlag = 1;
            cmdLen  = 0;
        }

        HAL_UART_Transmit(&huart3, (uint8_t *)"Received: ", 10, HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart3, (uint8_t *)cmdBuf, strlen(cmdBuf), HAL_MAX_DELAY);
        // Réarme tout de suite la réception
        HAL_UART_Receive_IT(huart, &rxChar, 1);
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}

#endif /* USE_FULL_ASSERT */
