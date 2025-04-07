#ifndef SERIAL_H
#define SERIAL_H

#include "main.h"

void SERIAL_Init(void);
void SERIAL_SendString(const char *str);
void SERIAL_receiveString(char *buffer, uint16_t size);

#endif // SERIAL_H