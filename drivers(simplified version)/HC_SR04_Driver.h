#ifndef __HC_SR04_DRIVER_H
#define __HC_SR04_DRIVER_H

#include "Delay.h"
#include "stdint.h"
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

void TIM1_Init(void);
void HC_SR04_Init(void);
float HC_SR04_GetDistance(void);

#endif