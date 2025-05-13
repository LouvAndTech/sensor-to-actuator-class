#include "main.h"
#include "ultra_sonic.h"
#include "gpio.h"
#include "tim.h"
#include "stdio.h"
#include "usart.h"
#include "stm32f4xx_hal.h"

#define SOUND_SPEED 340.0f // Speed of sound in cm/s
#define TRIG_PULSE_DURATION 10 // Trigger pulse duration in microseconds

/* Static attribute */
static float distance = 0.0f;
static uint8_t echo_received = 0;

/* Private function prototypes */
void gpio_echo_Callback(void);
void send_trigger_pulse(void);

/**
 * @brief Initialize the DWT (Data Watchpoint and Trace) for microsecond delays
 */
void DWT_Init(void)
{
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable TRC
        DWT->CYCCNT = 0;                               // Reset the cycle counter
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;           // Enable the cycle counter
    }
}

/**
 * @brief Delay for a specified number of microseconds
 * @param us Number of microseconds to delay
 */
void Timer_Delay_us(TIM_HandleTypeDef *htim, uint32_t us)
{
    __HAL_TIM_SET_COUNTER(htim, 0); // Reset the timer counter
    uint32_t delay_ticks = us * (HAL_RCC_GetPCLK1Freq() / ((htim->Init.Prescaler + 1) * 1000000));

    while (__HAL_TIM_GET_COUNTER(htim) < delay_ticks);
}

/* Public functions */
/**
 * @brief Initialize the ultrasonic sensor
 * 
 */
void ULTRA_SONIC_Init(void)
{
    HAL_TIM_Base_Start(&htim3); // Start the timer for echo measurement
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2); // Start PWM for trigger pulse
    HAL_GPIO_RegisterGPIO_EXTICallback(gpio_echo_Callback, SonicSensor_Echo_Pin);
}

/**
 * @brief Get the distance from the ultrasonic sensor
 * 
 * @return float the distance in cm
 */
float ULTRA_SONIC_GetDistance(void)
{
    send_trigger_pulse(); // Send the trigger pulse
    uint32_t timeout = HAL_GetTick() + 50; // 50 ms timeout
    while (!echo_received && HAL_GetTick() < timeout);
    if (!echo_received) {
        return 0.0f; // No echo received
    }
    echo_received = 0;
    return distance;
}

/**
 * @brief Test the ultrasonic sensor alone
 * 
 */
void ULTRA_SONIC_test(void)
{
    float distance = ULTRA_SONIC_GetDistance();
    char buffer[50];
    int len = snprintf(buffer, sizeof(buffer), "Distance: %.2f cm\r\n", distance);
    HAL_UART_Transmit(&huart2, (uint8_t *)buffer, len, HAL_MAX_DELAY);
    HAL_Delay(1000);
}

/* Private functions */

/**
 * @brief Send a trigger pulse to the ultrasonic sensor using TIM4_CH2
 */
void send_trigger_pulse(void)
{

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = (TRIG_PULSE_DURATION * (HAL_RCC_GetPCLK1Freq() / ((htim4.Init.Prescaler + 1) * 1000000)));
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;


    HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2);

    // Start the PWM pulse
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);


    // Wait for the pulse to complete
    Timer_Delay_us(&htim3, TRIG_PULSE_DURATION);


    // Stop the PWM pulse
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
}

/**
 * @brief Callback for handling the echo signal
 * 
 * @param GPIO_Pin The GPIO pin that triggered the interrupt
 */
void gpio_echo_Callback(void)
{
    static uint32_t start_time = 0;
    uint32_t end_time = 0;

    if (HAL_GPIO_ReadPin(SonicSensor_Echo_GPIO_Port, SonicSensor_Echo_Pin) == GPIO_PIN_SET)
    {
        // Rising edge: save the start time
        start_time = __HAL_TIM_GET_COUNTER(&htim3);
    }
    else
    {
        // Falling edge: save the end time
        end_time = __HAL_TIM_GET_COUNTER(&htim3);

        // Calculate the pulse duration in microseconds
        uint32_t duration = (end_time >= start_time) ? (end_time - start_time)
                                                        : ((0xFFFF - start_time) + end_time + 1);

        // Calculate distance in cm
        distance = (duration * SOUND_SPEED) / (2.0f * 100000.0f); // Convert to cm
        echo_received = 1;
    }
    
}