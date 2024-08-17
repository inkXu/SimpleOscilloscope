/*
 * file: app.h
 * brief: Application header file
 */
#ifndef __APP_H
#define __APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported defines ----------------------------------------------------------*/
#define LENGTH           300
#define AXIS_START_X     2
#define HORIZONTAL_AXIS  78

/* Typedefs ------------------------------------------------------------------*/
typedef struct {
    uint8_t display_update: 1;
    uint8_t update_samp_val: 1;
    uint8_t waveform_update: 2;
    uint8_t pwm_state: 4;
} app_state_t;
typedef struct {
    uint32_t pwm_freq;
    uint16_t pwm_duty;
    uint16_t wave_vpp;
    uint32_t wave_samp_freq;
    int32_t wave_freq;
    uint16_t data[LENGTH];
} app_data_t;
typedef enum {
    APP_STATE_DISABLE = 0,
    APP_STATE_ENABLE
} state_t;
typedef enum {
    WAVEFORM_UPDATE_NONE = 0,         /* not update */
    WAVEFORM_UPDATE_DONE,             /* update */
    WAVEFORM_UPDATE_DISABLE           /* closed update */
} waveform_update_t;
typedef enum {
    PWM_STATE_NONE = 0,               /* do not make any change */
    PWM_STATE_UPDATE,                 /* PWM argument will update */
    PWM_STATE_CLOSE,                  /* PWM will close */
    PWM_STATE_OPEN,                   /* PWM will open */
    PWM_STATE_DISABLE                 /* PWM was disabled */
} pwm_state_t;

/* Macro defines -------------------------------------------------------------*/
#define APP_STATE_T app_state_t
#define APP_DATA_T  app_data_t

/* External variables --------------------------------------------------------*/
extern APP_DATA_T  app_data;
extern APP_STATE_T app_state;

/* Function prototypes -------------------------------------------------------*/
void AppInit(void);
void AppStart(void);
uint16_t ADCGetValue(uint8_t ADC_CHANNEL_x);

#ifdef __cplusplus
}
#endif


#endif /* __APP_H */
