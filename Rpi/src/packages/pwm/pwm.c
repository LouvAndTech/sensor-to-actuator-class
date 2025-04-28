#include "pwm.h"

#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define PERIOD_US 20000 // 20ms period (50Hz)
#define DEFAULT_DUTY_CYCLE_PERCENT 7.5

struct PWM_s{
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    int duty_cycle_us;
    int running;
    pthread_t thread;
};

// Function prototypes

static void run(PWM *pwm);
static int init(PWM *pwm, const char *chip_name, int line_number);

// Public functions

PWM* pwm_new(const char *chip_name, int line_number){
    PWM *pwm = malloc(sizeof(PWM));
    if (!pwm) {
        perror("Failed to allocate memory for PWM");
        return NULL;
    }
    init(pwm, chip_name, line_number);
    return pwm;
}

void pwm_set_duty_cycle(PWM *pwm, float duty_cycle_percent) {
    pwm->duty_cycle_us = PERIOD_US * duty_cycle_percent / 100;
}

void pwm_start(PWM *pwm) {
    pwm->running = 1;
    pthread_create(&pwm->thread, NULL, (void *(*)(void *))run, pwm);
    pthread_detach(pwm->thread);
}

void pwm_stop(PWM *pwm) {
    pwm->running = 0;
    gpiod_line_set_value(pwm->line, 0); // Ensure line is low
}

void pwm_free(PWM *pwm) {
    gpiod_line_release(pwm->line);
    gpiod_chip_close(pwm->chip);
    free(pwm);
}

//private function

static void run(PWM *pwm) {
    printf("on : %d - off : %d\n", pwm->duty_cycle_us, PERIOD_US - pwm->duty_cycle_us);
    while(pwm->running) {
        gpiod_line_set_value(pwm->line, 1);
        usleep(pwm->duty_cycle_us); // High time
        gpiod_line_set_value(pwm->line, 0);
        usleep(PERIOD_US - pwm->duty_cycle_us); // Low time
    }
}

static int init(PWM *pwm, const char *chip_name, int line_number) {
    pwm->chip = gpiod_chip_open(chip_name);
    if (!pwm->chip) {
        perror("Failed to open GPIO chip");
        return -1;
    }

    pwm->line = gpiod_chip_get_line(pwm->chip, line_number);
    if (!pwm->line) {
        perror("Failed to get GPIO line");
        gpiod_chip_close(pwm->chip);
        return -1;
    }

    if (gpiod_line_request_output(pwm->line, "pwm", 0) < 0) {
        perror("Failed to request line as output");
        gpiod_chip_close(pwm->chip);
        return -1;
    }

    pwm->running = 0;
    pwm->duty_cycle_us = PERIOD_US * DEFAULT_DUTY_CYCLE_PERCENT / 100; // Default to 7.5% duty cycle
    return 0;
}