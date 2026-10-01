#ifndef MAIN_H
#define MAIN_H

#ifndef NATIVE_TEST
#include "stm32f4xx_hal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void SystemClock_Config(void);
void Error_Handler(void);
void HAL_FSMC_MspInit(void);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
