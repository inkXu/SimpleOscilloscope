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
#include "systick.h"

/* defines ------------------------------------------------------*/

/* private variables --------------------------------------------*/
const uint8_t UI_MSG[][32] = {
    "Thanks for using",
    "Vpp: 0.00V  F: 0.000kHz",
    "PWM:OFF   0.00kHz    0%"
};

/* function prototypes ------------------------------------------*/
void AppInterface(void);
uint16_t ADCGetValue(uint8_t ADC_CHANNEL_x);
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
    app_data.wave_samp_freq = 500;     // 1 / (500 * 10 us) = 200 Hz
    app_data.waveform.start = 3;
    app_data.waveform.end = 159;

    app_state.pwm_state = PWM_STATE_DISABLE;
    app_state.display_update = APP_STATE_ENABLE;
    app_state.waveform_update = WAVEFORM_UPDATE_NONE;
    
    /* peripheral configuration */
    RCUConfig();
    GPIOConfig();
#if OPEN_ZLOG == 1
    USARTConfig();
#endif
    ADCConfig();
    SPIConfig();

    /* driver configuration */
    LCD_Init();
    
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
    EMLOG(LOG_DEBUG, "waveform_update: %d", app_state.waveform_update);
    EMLOG(LOG_DEBUG, "wave_samp_freq: %d", app_data.wave_samp_freq);

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
        case PWM_STATE_UPDATE:
            timer_counter_value_config(TIMER14 , 0);
            timer_channel_output_pulse_value_config(
                TIMER14, 
                TIMER_CH_0, 
                (uint32_t)(app_data.pwm_duty / 100.0f * PWM_PERIOD / app_data.pwm_freq) );
            app_state.pwm_state = PWM_STATE_NONE;
            app_state.display_update = APP_STATE_ENABLE;
            break;
        case PWM_STATE_OPEN:
            PWM_OPEN(app_data.pwm_freq, app_data.pwm_duty);
            KEY02_01_INTERRUPT_ENABLE();
            app_state.pwm_state = PWM_STATE_NONE;
            app_state.display_update = APP_STATE_ENABLE;
            break;
        case PWM_STATE_CLOSE:
            exti_interrupt_disable(EXTI_13);
            exti_interrupt_disable(EXTI_14);
            timer_disable(TIMER14);
            rcu_periph_clock_disable(RCU_TIMER14);
            app_state.pwm_state = PWM_STATE_DISABLE;
            app_state.display_update = APP_STATE_ENABLE;
            break;
        default:
            break;
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
    /* black background */
    LCD_Fill(0, 0, LCD_WIDTH, LCD_HEIGHT, BLACK);
    /* title */
    LCD_Show_String(12, 6, UI_MSG[0], RED, BLACK, 12, 1);
    /* coordinate axis */
    LCD_Draw_Line(3, 24, 3, 96, GREEN);
    LCD_Draw_Line(3, 96, LCD_WIDTH - 3, 96, GREEN);
    /* 0V horizontal line */
    LCD_Draw_Line(3, 60, LCD_WIDTH - 3, 60, PURPLE);
    /* input waveform state */
    LCD_Show_String(3, 102, UI_MSG[1], YELLOW, BLACK, 12, 1);
    /* PWM signal state */
    LCD_Show_String(3, 116, UI_MSG[2], PURPLE, BLACK, 12, 1);
}

/*
 * @brief  
 * @param  None
 * @retval None
 */
void AppStateDisplay(void)
{
    /* input signal */
    LCD_ShowFloatNum1(27, 102, (float)app_data.wave_vpp / 100, 4, YELLOW, BLACK, 12);     // vpp
    LCD_ShowFloatNum1(87, 102, (float)app_data.wave_freq / 1000, 5, YELLOW, BLACK, 12);   // frequecny
    /* PWM */
    if (app_state.pwm_state != PWM_STATE_DISABLE) {                                       // pwm switch state
        LCD_Show_String(27, 116, (uint8_t *)"ON ", PURPLE, BLACK, 12, 0);
    } else {
        LCD_Show_String(27, 116, (uint8_t *)"OFF", PURPLE, BLACK, 12, 0);
    }
    LCD_ShowFloatNum1(57, 116, (float)app_data.pwm_freq / 1000, 3, PURPLE, BLACK, 12);    // pwm frequency
    LCD_ShowIntNum(117, 116, (uint16_t)app_data.pwm_duty, 3, PURPLE, BLACK, 12);          // pwm duty
}

/*
 * @brief  
 * @param  None
 * @retval None
 */
void WaveformDisplay(void)
{    
    int16_t i, x;
    float voltage;

    EMLOG(LOG_DEBUG, "function WaveformDisplay exeute...");

    if (app_data.waveform.start + 1 >= LENGTH) {
        app_data.waveform.start = 0;
        app_data.waveform.end ++;
    } else if (app_data.waveform.end + 1 >= LENGTH) {
        app_data.waveform.end = 0;
        app_data.waveform.start++;
    } else {
        app_data.waveform.start++;
        app_data.waveform.end++;
    }
    voltage = ADCGetValue(ADC_CHANNEL_3) / 4096.0f;
    app_data.waveform.data[app_data.waveform.end] = (int16_t)(voltage * 32 + 0.5f);
    
    if (voltage > app_data.wave_vpp)
        app_data.wave_vpp = (int32_t)(voltage * 3.3 * 100);

    i = app_data.waveform.start;
    LCD_Fill(4, 24, LCD_WIDTH, 95, BLACK);
    while (i != app_data.waveform.end) {
        x = LCD_Draw_Curve(3, 60, app_data.waveform.data[i++]);
        LCD_Draw_Point(x, 60, PURPLE);
        if (i >= LENGTH) {
            i = 0;
        }
    }
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

/*
 * @brief    get ADC sampling value
 * @param    channel: specify the ADC channel
 * @retval   16-bit adc sampling value
 */
uint16_t ADCGetValue(uint8_t ADC_CHANNEL_x)
{
    uint16_t adc_value = 0;

    adc_regular_channel_config(0, ADC_CHANNEL_x, ADC_SAMPLETIME_55POINT5);
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);

    while (adc_flag_get(ADC_FLAG_EOC) == RESET);
    adc_value = adc_regular_data_read();

    return adc_value;
}
