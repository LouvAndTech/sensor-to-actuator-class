#include <stdio.h>
#include <unistd.h>
#include "./packages/pwm/pwm.h"


#define CHIP_NAME "/dev/gpiochip0"
#define LINE_NUMBER 12 // Change this to the appropriate GPIO line number

int main() {
    printf("Starting API PWM example...\n");

    // Initialize the PWM signal
    PWM *pwm = pwm_new(CHIP_NAME, LINE_NUMBER);
    if (!pwm) {
        fprintf(stderr, "Failed to initialize PWM\n");
        return 1;
    }

    // Set the duty cycle to 10.5%
    pwm_set_duty_cycle(pwm, 7.5);
    pwm_start(pwm);
    printf("PWM signal started with 7.5%% duty cycle.\n");


    pwm_set_duty_cycle(pwm, 5); // 0°
    printf("Servo to 0°.\n");
    sleep(10); 
    pwm_set_duty_cycle(pwm, 7.5); // 90°
    printf("Servo to 90°.\n");
    sleep(10); 
    pwm_set_duty_cycle(pwm, 10); // 180°
    printf("Servo to 180°.\n");
    sleep(10);


    // Stop the PWM signal
    pwm_stop(pwm);
    printf("PWM signal stopped.\n");

    // Free the PWM resources
    pwm_free(pwm);
    printf("PWM resources cleaned up.\n");
    return 0;
}