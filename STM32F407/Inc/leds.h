#ifndef __LEDS_H__
#define __LEDS_H__

#ifdef __cplusplus
extern "C" {
#endif

void LEDS_Init(void);
void LEDS_SetBlue(void);
void LEDS_SetGreen(void);
void LEDS_test(void);
void callback_chenillar(void);

#ifdef __cplusplus
}
#endif

#endif // LEDS_H