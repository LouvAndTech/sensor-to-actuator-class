#include "main.h"
#include "ultra_sonic.h"
#include "gpio.h"
#include "tim.h"
#include "stdio.h"
#include "usart.h"

/* Static attribute */
float distance = 0.0f;

/* Private function prototypes */
void gpio_trigger_Callback(void);

/* Public functions */
void ULTRA_SONIC_Init(void)
{
    // Initialize the ultrasonic sensor
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
    GPIO_RegisterGPIO_EXTICallback(gpio_trigger_Callback, SonicSensor_Echo_Pin);
}

float ULTRA_SONIC_GetDistance(void)
{
    return distance; 
}

void ULTRA_SONIC_test(void){
    float distance = ULTRA_SONIC_GetDistance();
    char buffer[50];
    int len = snprintf(buffer, sizeof(buffer), "Distance: %f \r\n", distance);
    HAL_UART_Transmit(&huart2, (uint8_t *)buffer, len, HAL_MAX_DELAY);
    HAL_Delay(1000);
}

/* Private functions */

void gpio_trigger_Callback() {
    static uint32_t start_time = 0;
    // Read the value of the pin 
    int8_t echo = HAL_GPIO_ReadPin(SonicSensor_Echo_GPIO_Port, SonicSensor_Echo_Pin);
    if (echo){
        // Save the start time
        start_time = HAL_GetTick();
    } else {
        // Save the end time
        uint32_t duration = HAL_GetTick() - start_time;
        // Calculate distance in cm
        distance = (duration * 340)/100/2;
    }
}

