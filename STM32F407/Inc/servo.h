#ifndef SERVO_H
#define SERVO_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

void SERVO_Init(void);
void SERVO_set_servo_percentage(uint8_t percentage);

#ifdef __cplusplus
}
#endif

#endif // SERVO_H