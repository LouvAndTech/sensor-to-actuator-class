#ifndef BUTTON_H
#define BUTTON_H

#include "gpio.h"

// Prototypes des fonctions
void BUTTON_Init(void);
uint8_t BUTTON_GetState(void);
void BUTTON_reset(void);

#endif // BUTTON_H