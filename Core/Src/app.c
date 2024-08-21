/*
 * file: app.c
 * brief: Application main file
 */
/* includes -----------------------------------------------------*/
#include "app.h"
#include "log.h"
#include "lcd.h"
#include "peripheral.h"
#include "gd32e23x.h"
#include "stdint.h"
#include <string.h>
#include "systick.h"

/* defines ------------------------------------------------------*/

/* private variables --------------------------------------------*/
const uint8_t UI_MSG[][32] = {
    "Oscilloscope",
    "Vpp: 0.00V  F: 0.00kHz",
    "PWM:OFF   0.00Hz    0%"
};

/* function prototypes ------------------------------------------*/
void AppInterface(void);
void WaveformDisplay(void);
void AppStateDisplay(void);

/* external variables -------------------------------------------*/
APP_DATA_T  app_data;
APP_STATE_T app_state;

/*
 * @brief:  Application initialization
 * @param:  None
 * @retval: None
 */
void AppInit(void)
{
    /* program data initialization */
    app_data.pwm_freq = 50;
    app_data.pwm_duty = 50;
    app_data.wave_freq = 0;
    app_data.wave_vpp = 0;
    app_data.wave_samp_freq = ADC_SAMPLETIME_239POINT5;

    app_state.pwm_state = PWM_STATE_DISABLE;
    app_state.display_update = APP_STATE_DISABLE;
    app_state.update_samp_val = APP_STATE_DISABLE;
    app_state.waveform_update = WAVEFORM_UPDATE_NONE;
    
    /* peripheral configuration */
    RCUConfig();
    GPIOConfig();
#if OPEN_ZLOG == 1
    USARTConfig();
#endif
    ADCConfig();
    DMAConfig();
    SPIConfig();

    /* driver configuration */
    LCD_Init();
    delay_1ms(10);
    
    AppInterface();
    WaveformDisplay();
    AppStateDisplay();
    
    TIMConfig();

    /* load message */
    EMLOG(LOG_DEBUG, "App Start!");
}

/*
 * @brief:  Application main loop
 * @param:  None
 * @retval: None
 */
void AppStart(void)
{
    EMLOG(LOG_DEBUG, "/***************State****************/");
    EMLOG(LOG_DEBUG, "display_update: %d", app_state.display_update);
    EMLOG(LOG_DEBUG, "update_samp_val: %d", app_state.update_samp_val);
    EMLOG(LOG_DEBUG, "waveform_update: %d", app_state.waveform_update);
    EMLOG(LOG_INFO, "pwm_state: %d", app_state.pwm_state);

    EMLOG(LOG_DEBUG, "/***************Data*****************/");
    EMLOG(LOG_DEBUG, "pwm_freq: %d", app_data.pwm_freq);
    EMLOG(LOG_DEBUG, "pwm_duty: %d", app_data.pwm_duty);
    EMLOG(LOG_DEBUG, "wave_freq: %d", app_data.wave_freq);
    EMLOG(LOG_DEBUG, "wave_vpp: %d", app_data.wave_vpp);
    EMLOG(LOG_DEBUG, "wave_samp_freq: %d", app_data.wave_samp_freq);

    EMLOG(LOG_INFO, "pwm_freq: %d", app_data.pwm_freq);
    EMLOG(LOG_INFO, "pwm_freq: %d", app_data.pwm_duty);
    EMLOG(LOG_INFO, "wave_freq: %d", app_data.wave_freq);

    if (RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_9)) {
        delay_1ms(10);
        if (RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_9)) {
            while (RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_9));
            if (app_state.waveform_update != WAVEFORM_UPDATE_DISABLE) {
                app_state.waveform_update = WAVEFORM_UPDATE_DISABLE;
            } else {
                app_state.waveform_update = WAVEFORM_UPDATE_NONE;
            }
        }
    }
    switch (app_state.pwm_state) {
        case PWM_STATE_OPEN:
            PWM_OPEN();
            KEY02_01_INTERRUPT_ENABLE();
            // app_state.pwm_state = PWM_STATE_NONE;
            // app_state.display_update = APP_STATE_ENABLE;
            // break;
        case PWM_STATE_UPDATE:
            timer_autoreload_value_config(TIMER14, app_data.pwm_freq);
            timer_channel_output_pulse_value_config(TIMER14, TIMER_CH_0, app_data.pwm_freq * app_data.pwm_duty / 100);
            app_state.pwm_state = PWM_STATE_NONE;
            app_state.display_update = APP_STATE_ENABLE;
            break;
        case PWM_STATE_CLOSE:
            exti_interrupt_disable(EXTI_13);
            exti_interrupt_disable(EXTI_14);
            timer_disable(TIMER14);
            rcu_periph_clock_disable(RCU_TIMER14);
            gpio_bit_reset(GPIOA, GPIO_PIN_2);
            app_state.pwm_state = PWM_STATE_DISABLE;
            app_state.display_update = APP_STATE_ENABLE;
            break;
        default:
            break;
    }
    if (app_state.update_samp_val == APP_STATE_ENABLE) {
        adc_regular_channel_config(0, ADC_CHANNEL_3, app_data.wave_samp_freq);
        app_state.update_samp_val = APP_STATE_DISABLE;
        app_state.display_update = APP_STATE_ENABLE;
    }
    if (app_state.waveform_update == WAVEFORM_UPDATE_DONE) {
        app_state.waveform_update = WAVEFORM_UPDATE_NONE;
        WaveformDisplay();
    }
    if (app_state.display_update) {
        app_state.display_update = APP_STATE_DISABLE;
        AppStateDisplay();
    }
}

/*
 * @brief  application user interface
 * @param  None
 * @retval None
 */
void AppInterface(void)
{
    LCD_Draw_Point(AXIS_START_X, HORIZONTAL_AXIS, GREEN);

    /* black background */
    LCD_Fill(0, 0, LCD_WIDTH, LCD_HEIGHT, BLACK);
    /* title */
    LCD_Show_String(12, 4, UI_MSG[0], RED, BLACK, 16, 1);
    /* sampling level */
    LCD_Show_String(120, 8, (uint8_t *)"----I", YELLOW, BLACK, 12, 0);
    /* input waveform state */
    LCD_Show_String(AXIS_START_X + 1, 102, UI_MSG[1], YELLOW, BLACK, 12, 1);
    /* PWM signal state */
    LCD_Show_String(AXIS_START_X + 1, 116, UI_MSG[2], PURPLE, BLACK, 12, 1);
    /* coordinate axis */
    LCD_Draw_Line(AXIS_START_X, 24, AXIS_START_X, 96, GREEN);
    LCD_Draw_Line(AXIS_START_X, 96, LCD_WIDTH - AXIS_START_X, 96, GREEN);
    /* 0V horizontal line */
    LCD_Draw_Line(AXIS_START_X + 1, HORIZONTAL_AXIS, LCD_WIDTH - AXIS_START_X, HORIZONTAL_AXIS, PURPLE);
}

/*
 * @brief  
 * @param  None
 * @retval None
 */
void AppStateDisplay(void)
{
    char buf[6] = "";

    EMLOG(LOG_DEBUG, "function AppStateDisplay exeute...");

    /* sampling level */
    switch (app_data.wave_samp_freq)
    {
        case ADC_SAMPLETIME_28POINT5:
            strncpy(buf, "----I", 6);
            break;
        case ADC_SAMPLETIME_41POINT5:
            strncpy(buf, "---I-", 6);
            break;
        case ADC_SAMPLETIME_55POINT5:
            strncpy(buf, "--I--", 6);
            break;
        case ADC_SAMPLETIME_71POINT5:
            strncpy(buf, "-I---", 6);
            break;
        default:
            strncpy(buf, "I----", 6);
            break;
    }
    LCD_Show_String(120, 8, (uint8_t *)buf, YELLOW, BLACK, 12, 0);
    /* input signal parameter */
    LCD_ShowFloatNum1(27, 102, (float)(5 - app_data.wave_vpp / 4096.0f * 6.6f - 0.28f), 4, YELLOW, BLACK, 12);     // vpp
    LCD_ShowFloatNum1(87, 102, (float)(app_data.wave_freq * 1.03f / 1000.0f), 4, YELLOW, BLACK, 12);               // frequecny
    app_data.wave_freq = 0;
    /* PWM */
    if (app_state.pwm_state != PWM_STATE_DISABLE) {                                            // pwm switch state
        LCD_Show_String(27, 116, (uint8_t *)"ON ", PURPLE, BLACK, 12, 0);
    } else {
        LCD_Show_String(27, 116, (uint8_t *)"OFF", PURPLE, BLACK, 12, 0);
    }
    LCD_ShowIntNum(57, 116, (uint16_t)(100000U / app_data.pwm_freq), 5, PURPLE, BLACK, 12);    // pwm frequency
    LCD_ShowIntNum(111, 116, (uint16_t)app_data.pwm_duty, 3, PURPLE, BLACK, 12);               // pwm duty
}

/**
 * @brief be used to display and update waveform
 * 
 */
void WaveformDisplay(void)
{    
    int16_t i, x, start_point;

    EMLOG(LOG_INFO, "function WaveformDisplay exeute...");

    for (start_point = 0; start_point < LENGTH - LCD_WIDTH - AXIS_START_X; start_point ++) {
        if (app_data.data[start_point] > 3104) {
            for (; start_point < LENGTH - LCD_WIDTH; start_point ++) {
                if (app_data.data[start_point] < 3104) {
                    break;
                }
            }
            break;
        }
    }

    // LCD_Fill(4, 24, LCD_WIDTH, 95, BLACK);
    // LCD_Draw_Point(0, HORIZONTAL_AXIS, GREEN);
    app_data.wave_vpp = 2929;
    for (i = start_point; i < start_point + 157; i++) {
        if (app_data.data[i] < 3104) {
            if (app_data.wave_vpp > app_data.data[i]) app_data.wave_vpp = app_data.data[i];
            x = LCD_Draw_Curve(AXIS_START_X + 1, HORIZONTAL_AXIS, (int16_t)((1 - app_data.data[i] / 3104.0f) * 53.0f));
        } else {
            x = LCD_Draw_Curve(AXIS_START_X + 1, HORIZONTAL_AXIS, (int16_t)((app_data.data[i] - 3104) / 992.0f * -17.0f));
        }
        app_data.data[i] = 0;
        LCD_Draw_Point(x, HORIZONTAL_AXIS, PURPLE);
        // delay_1ms(2);
    }
    dma_transfer_number_config(DMA_CH0, LENGTH);
    dma_channel_enable(DMA_CH0);
}

#if OPEN_ZLOG == 1
/* 
 * @brief    retarget the C library printf function to the USART
 * @param    ch: character to be printed
 * @retval   none
 */
int __io_putchar(int ch)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));

    return ch;
}
#endif

// /*
//  * @brief    get ADC sampling value
//  * @param    channel: specify the ADC channel
//  * @retval   16-bit adc sampling value
//  */
// uint16_t ADCGetValue(uint8_t ADC_CHANNEL_x)
// {
//     uint16_t adc_value = 0;

//     adc_regular_channel_config(0, ADC_CHANNEL_x, ADC_SAMPLETIME_55POINT5);
//     adc_software_trigger_enable(ADC_REGULAR_CHANNEL);

//     while (adc_flag_get(ADC_FLAG_EOC) == RESET);
//     adc_value = adc_regular_data_read();

//     return adc_value;
// }
