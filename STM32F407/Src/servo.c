#include "servo.h"
#include "tim.h"

#define MIN_DUTY 2000
#define MAX_DUTY 4000


/* Public functions */

/**
 * @brief Init the servo motor
 * 
 */
void SERVO_Init(void)
{
    // Initialize the servo motor
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, MIN_DUTY);
}

/**
 * @brief Set the servo motor to a percentage of its range
 * 
 * @param percentage The percentage to set the servo motor to (0-100)
 */
void SERVO_set_servo_percentage(uint8_t percentage)
{
    // Set the duty cycle based on the percentage
    uint32_t duty_cycle = MIN_DUTY + (MAX_DUTY - MIN_DUTY) * percentage / 100;
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, duty_cycle);
}