#ifndef PWM_H
#define PWM_H

typedef struct PWM_s PWM;

PWM* pwm_new(const char *chip_name, int line_number);
void pwm_set_duty_cycle(PWM *pwm, float duty_cycle_percent);
void pwm_start(PWM *pwm);
void pwm_stop(PWM *pwm);
void pwm_free(PWM *pwm);

#endif // PWM_H