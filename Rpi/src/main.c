#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>

#define CHIP_NAME "/dev/gpiochip0"
#define PWM_LINE 12
#define PERIOD_NS 20000000 // 20ms period (50Hz)
#define DUTY_CYCLE_PERCENT 7.5 // 7.5% duty cycle for neutral position
#define DUTY_CYCLE_NS (PERIOD_NS * DUTY_CYCLE_PERCENT / 100)

int main() {
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    int ret;

    // Open GPIO chip
    chip = gpiod_chip_open(CHIP_NAME);
    if (!chip) {
        perror("Failed to open GPIO chip");
        return 1;
    }

    // Get GPIO line
    line = gpiod_chip_get_line(chip, PWM_LINE);
    if (!line) {
        perror("Failed to get GPIO line");
        gpiod_chip_close(chip);
        return 1;
    }

    // Request line as output
    ret = gpiod_line_request_output(line, "pwm", 0);
    if (ret < 0) {
        perror("Failed to request line as output");
        gpiod_chip_close(chip);
        return 1;
    }

    // Generate PWM signal
    for (int i = 0; i < 100; i++) { // Run for 100 cycles
        gpiod_line_set_value(line, 1);
        usleep(DUTY_CYCLE_NS / 1000); // High time
        gpiod_line_set_value(line, 0);
        usleep((PERIOD_NS - DUTY_CYCLE_NS) / 1000); // Low time
    }

    // Release line and close chip
    gpiod_line_release(line);
    gpiod_chip_close(chip);

    printf("PWM signal generated.\n");
    return 0;
}