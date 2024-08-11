/*!
    \file    gd32e230c_eval.c
    \brief   firmware functions to manage leds, keys, COM ports
    
    \version 2024-02-22, V2.1.0, firmware for GD32E23x
*/

/*
    Copyright (c) 2024, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32e230c_eval.h"
#include "systick.h"

/* private variables */

// static const rcu_periph_enum COM_CLK[COMn]  = {EVAL_COM_CLK};

// static const uint32_t COM_TX_PIN[COMn]      = {EVAL_COM_TX_PIN};

// static const uint32_t COM_RX_PIN[COMn]      = {EVAL_COM_RX_PIN};

// static const uint32_t KEY_PORT[KEYn]        = {WAKEUP_KEY_GPIO_PORT, 
//                                                TAMPER_KEY_GPIO_PORT
//                                                };

// static const uint32_t KEY_PIN[KEYn]         = {WAKEUP_KEY_PIN, 
//                                                TAMPER_KEY_PIN
//                                                };

// static const rcu_periph_enum KEY_CLK[KEYn]  = {WAKEUP_KEY_GPIO_CLK, 
//                                                TAMPER_KEY_GPIO_CLK
//                                                };

// static const exti_line_enum KEY_EXTI_LINE[KEYn] = {WAKEUP_KEY_EXTI_LINE,
//                                                    TAMPER_KEY_EXTI_LINE
//                                                    };

// static const uint8_t KEY_PORT_SOURCE[KEYn]      = {WAKEUP_KEY_EXTI_PORT_SOURCE,
//                                                    TAMPER_KEY_EXTI_PORT_SOURCE
//                                                    };

// static const uint8_t KEY_PIN_SOURCE[KEYn]       = {WAKEUP_KEY_EXTI_PIN_SOURCE,
//                                                    TAMPER_KEY_EXTI_PIN_SOURCE
//                                                    };

// static const uint8_t KEY_IRQn[KEYn]             = {WAKEUP_KEY_EXTI_IRQn, 
//                                                    TAMPER_KEY_EXTI_IRQn
//                                                    };

/* eval board low layer private functions */
/*
 * @brief      GPIO Initialization
 * @param      None
 * @retval     None
 */
void GPIO_Init(void)
{
    /* enable the led clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable the key clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable the system clock */
    rcu_periph_clock_enable(RCU_CFGCMP);

    /* configure led GPIO port */ 
    gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
    /* configure key GPIO port */ 
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_9|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);

    /* enable the NVIC and configure the priority */
    nvic_irq_enable(EXTI4_15_IRQn, 2U);
    /* connect key EXTI line to key GPIO pin */
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN4);
    /* configure key EXTI line */
    exti_init(EXTI_4, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_4);
}

/*
 * @brief    USART Initialization
 * @param    None
 * @retval   None
 */
void USART_Init(void)
{
    /* enable the USART clock */
    rcu_periph_clock_enable(RCU_GPIOA);

     /* enable the USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* alternate function configuration */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9|GPIO_PIN_10);
    
    /* configure USART GPIO port */ 
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9|GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9|GPIO_PIN_10);

    /* deinitilize the USART */
    usart_deinit(USART0);

    /* USART configure */
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_baudrate_set(USART0, 115200U);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);
}

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

/*
 * @brief    ADC Initialization
 * @param    None
 * @retval   None
 */
void ADC_Init(void)
{
    /* enable the ADC clock */
    // rcu_periph_clock_enable(RCU_GPIOA);

     /* enable the ADC clock */
    rcu_periph_clock_enable(RCU_ADC);

    /* configure the ADC frequency */
    rcu_adc_clock_config(RCU_ADCCK_AHB_DIV3);
    
    /* configure ADC GPIO port */ 
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_3);

    /* disable the scan mode and continuous mode, use the single conversion mode */
    adc_special_function_config(ADC_SCAN_MODE, DISABLE);
    adc_special_function_config(ADC_CONTINUOUS_MODE, DISABLE);
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);

    adc_regular_channel_config(0U, ADC_CHANNEL_3, ADC_SAMPLETIME_55POINT5);
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    adc_resolution_config(ADC_RESOLUTION_12B);
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1);

    adc_external_trigger_config(ADC_REGULAR_CHANNEL,ENABLE);
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);

    adc_enable();
    delay_1ms(1U);

    adc_calibration_enable();
}

/*
 * @brief    ADC 
 * @param    channel: specify the ADC channel
 * @retval   None
 */
uint16_t ADCGetValue(uint8_t ADC_CHANNEL_x)
{
    uint16_t adc_value = 0;
        
    //设置采集通道
    adc_regular_channel_config(0, ADC_CHANNEL_x, ADC_SAMPLETIME_55POINT5);
    //开始软件转换
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);

    //等待 ADC 采样完成 
    while (adc_flag_get(ADC_FLAG_EOC) == RESET);
    //读取采样值
    adc_value = adc_regular_data_read();
    //返回采样值
    return adc_value;
}


void Timer_Init(void)
{
    // 定时器初始化参数结构体
    timer_parameter_struct timer_initpara;
    timer_oc_parameter_struct timer_ocinitpara;
    timer_ic_parameter_struct timer_icinitpara;

    /* enable the timer clock */
    rcu_periph_clock_enable(RCU_TIMER15);
    rcu_periph_clock_enable(RCU_TIMER14);
    rcu_periph_clock_enable(RCU_TIMER2);

    /* configure TIMER GPIO port */ 
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    /* alternate function configuration */
    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_2);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_6);

    // reset timer configuration
    timer_deinit(TIMER15);
    timer_deinit(TIMER14);
    timer_deinit(TIMER2);
    // initialize timer 15
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 7200 - 1;
    timer_initpara.period            = 10000 - 1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER15, &timer_initpara);
    // initialize timer 14
    // timer_struct_para_init(&timer_initpara);
    // timer_initpara.prescaler = 7200 - 1;
    // timer_initpara.period = 10000 - 1;
    // timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    // timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    // timer_initpara.counterdirection = TIMER_COUNTER_UP;
    // timer_initpara.repetitioncounter = 0;
    timer_init(TIMER14, &timer_initpara);
    // initialize timer 2
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 72-1;
    timer_initpara.period            = 65535;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2, &timer_initpara);

    timer_channel_output_struct_para_init(&timer_ocinitpara);
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER14, TIMER_CH_0, &timer_ocinitpara);
    timer_channel_output_pulse_value_config(TIMER14, TIMER_CH_0, 5000);
    /* PWM0: vaild to invaild level, PWM1: invaild to vaild level */
    timer_channel_output_mode_config(TIMER14, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER14, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
    timer_auto_reload_shadow_enable(TIMER14);
    timer_primary_output_config(TIMER14, ENABLE);
    
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_RISING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0x0;
    timer_input_capture_config(TIMER2, TIMER_CH_0, &timer_icinitpara);
    timer_auto_reload_shadow_enable(TIMER2);

    /* enable timer interrupt */
    nvic_irq_enable(TIMER15_IRQn, 1U);
    nvic_irq_enable(TIMER2_IRQn, 0U);
    timer_interrupt_flag_clear(TIMER15, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(TIMER15, TIMER_INT_UP);
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_CH0);
    timer_interrupt_enable(TIMER2, TIMER_INT_CH0);

    timer_enable(TIMER15);
    timer_enable(TIMER14);
    timer_enable(TIMER2);
}
// /*!
//     \brief      configure key
//     \param[in]  keynum: specify the key to be configured
//       \arg        KEY_TAMPER: tamper key
//       \arg        KEY_WAKEUP: wakeup key
//     \param[in]  keymode: specify button mode
//       \arg        KEY_MODE_GPIO: key will be used as simple IO
//       \arg        KEY_MODE_EXTI: key will be connected to EXTI line with interrupt
//     \param[out] none
//     \retval     none
// */
// void gd_eval_key_init(key_typedef_enum keynum, keymode_typedef_enum keymode)
// {
//     /* enable the key clock */
//     rcu_periph_clock_enable(KEY_CLK[keynum]);
//     rcu_periph_clock_enable(RCU_CFGCMP);

//     /* configure button pin as input */
//     gpio_mode_set(KEY_PORT[keynum], GPIO_MODE_INPUT, GPIO_PUPD_NONE, KEY_PIN[keynum]);

//     if (keymode == KEY_MODE_EXTI) {
//         /* enable and set key EXTI interrupt to the lowest priority */
//         nvic_irq_enable(KEY_IRQn[keynum], 2U);

//         /* connect key EXTI line to key GPIO pin */
//         syscfg_exti_line_config(KEY_PORT_SOURCE[keynum], KEY_PIN_SOURCE[keynum]);

//         /* configure key EXTI line */
//         exti_init(KEY_EXTI_LINE[keynum], EXTI_INTERRUPT, EXTI_TRIG_FALLING);
//         exti_interrupt_flag_clear(KEY_EXTI_LINE[keynum]);
//     }
// }

// /*!
//     \brief      return the selected key state
//     \param[in]  keynum: specify the key to be checked
//       \arg        KEY_TAMPER: tamper key
//       \arg        KEY_WAKEUP: wakeup key
//     \param[out] none
//     \retval     the key's GPIO pin value
// */
// uint8_t gd_eval_key_state_get(key_typedef_enum keynum)
// {
//     return gpio_input_bit_get(KEY_PORT[keynum], KEY_PIN[keynum]);
// }

// /*!
//     \brief      configure COM port
//     \param[in]  com: COM on the board
//       \arg        EVAL_COM: COM on the board
//     \param[out] none
//     \retval     none
// */
// void gd_eval_com_init(uint32_t com)
// {
//     uint32_t COM_ID;
    
//     COM_ID = 0U;

//     /* enable COM GPIO clock */
//     rcu_periph_clock_enable(EVAL_COM_GPIO_CLK);

//     /* enable USART clock */
//     rcu_periph_clock_enable(COM_CLK[COM_ID]);

//     /* connect port to USARTx_Tx */
//     gpio_af_set(EVAL_COM_GPIO_PORT, EVAL_COM_AF, COM_TX_PIN[COM_ID]);

//     /* connect port to USARTx_Rx */
//     gpio_af_set(EVAL_COM_GPIO_PORT, EVAL_COM_AF, COM_RX_PIN[COM_ID]);

//     /* configure USART Tx as alternate function push-pull */
//     gpio_mode_set(EVAL_COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, COM_TX_PIN[COM_ID]);
//     gpio_output_options_set(EVAL_COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, COM_TX_PIN[COM_ID]);

//     /* configure USART Rx as alternate function push-pull */
//     gpio_mode_set(EVAL_COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, COM_RX_PIN[COM_ID]);
//     gpio_output_options_set(EVAL_COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, COM_RX_PIN[COM_ID]);

//     /* USART configure */
//     usart_deinit(com);
//     usart_baudrate_set(com, 115200U);
//     usart_receive_config(com, USART_RECEIVE_ENABLE);
//     usart_transmit_config(com, USART_TRANSMIT_ENABLE);

//     usart_enable(com);
// }
