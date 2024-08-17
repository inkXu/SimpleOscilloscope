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
void DMAConfig(void);
void SPIConfig(void);
#if OPEN_ZLOG == 1
void USARTConfig(void);
#endif
void Timer14Config();                              // PWM
void GPIO13_14InterruptEnable(void);               // GPIO13: PA13, GPIO14: PA14

/* defines */
#define PWM_OPEN()                  Timer14Config()
#define KEY02_01_INTERRUPT_ENABLE() GPIO13_14InterruptEnable()

#ifdef __cplusplus
}
#endif

#endif /* __PERIPHERAL_H */