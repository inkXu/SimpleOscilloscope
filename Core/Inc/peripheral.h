#ifndef __PERIPHERAL_H
#define __PERIPHERAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "log.h"
#include "stdint.h"

/* function prototype */
void RCUConfig(void);
void GPIOConfig(void);
void TIMConfig(void);
void ADCConfig(void);
void SPIConfig(void);
#if OPEN_ZLOG == 1
void USARTConfig(void);
#endif
void Timer14Config(uint32_t freq, uint32_t duty); // freq: 1-1000 Hz, duty: 0-100%
void GPIO13_14InterruptEnable(void);               // GPIO13: PA13, GPIO14: PA14

/* defines */
#define PWM_PERIOD                  200000
#define PWM_OPEN(FREQ, DUTY)        Timer14Config(FREQ, DUTY)
#define KEY02_01_INTERRUPT_ENABLE() GPIO13_14InterruptEnable()

#ifdef __cplusplus
}
#endif

#endif /* __PERIPHERAL_H */