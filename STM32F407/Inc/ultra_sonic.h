#ifndef __ULTRA_SONIC_H__
#define __ULTRA_SONIC_H__

#ifdef __cplusplus
extern "C" {
#endif

void ULTRA_SONIC_Init(void);
float ULTRA_SONIC_GetDistance(void);
void ULTRA_SONIC_test(void);

#ifdef __cplusplus
}
#endif


#endif // ULTRA_SONIC_H