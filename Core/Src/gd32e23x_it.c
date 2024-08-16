/*!
    \file    gd32e23x_it.c
    \brief   interrupt service routines

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

#include "gd32e23x_it.h"
#include "systick.h"
#include "app.h"
#include "log.h"

uint8_t rotate_flag;
uint8_t timer_flag;
uint32_t time_value;
static uint32_t value;

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
    /* if NMI exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
    /* if SVC exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
    /* if PendSV exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    // led_spark();
    delay_decrement();
}

/*
 * @brief    this function handles EXTI4 exception
 * @param    None
 * @retval   None
 */
void EXTI4_15_IRQHandler(void)
{
    if (SET == exti_interrupt_flag_get(EXTI_4)) {
        exti_interrupt_flag_clear(EXTI_4);
        if (app_state.waveform_update == WAVEFORM_UPDATE_DISABLE) return ;
        if (RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_3)) {
            // turn right
            if (app_data.wave_samp_freq > 50) {
                app_data.wave_samp_freq -= 1;
                app_state.waveform_update = WAVEFORM_UPDATE_DONE;
            }
        } else {
            // turn left
            if (app_data.wave_samp_freq < 500) {
                app_data.wave_samp_freq += 1;
                app_state.waveform_update = WAVEFORM_UPDATE_DONE;
            }
        }
    } else if (SET == exti_interrupt_flag_get(EXTI_13)) {
        exti_interrupt_flag_clear(EXTI_13);
        if (RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_13)) {
            if (app_state.pwm_state == PWM_STATE_DISABLE) return ;
            if (app_data.pwm_duty >= 100) {
                app_data.pwm_duty = 0;
            } else {
                app_data.pwm_duty += 5;
            }
            app_state.pwm_state = PWM_STATE_UPDATE;
        }
    } else if (SET == exti_interrupt_flag_get(EXTI_14)) {
        exti_interrupt_flag_clear(EXTI_14);
        if (app_state.pwm_state == PWM_STATE_DISABLE) return ;
        if (RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_14)) {
            if (app_data.pwm_freq >= 10000) {
                app_data.pwm_freq = 50;
            } else {
                app_data.pwm_freq += 50;
            }
            app_state.pwm_state = PWM_STATE_UPDATE;
        }
    } else if (SET == exti_interrupt_flag_get(EXTI_15)) {
        exti_interrupt_flag_clear(EXTI_15);
        if (RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_15)) {
            if (app_state.pwm_state != PWM_STATE_DISABLE) {
                app_state.pwm_state = PWM_STATE_CLOSE;
            } else {
                app_state.pwm_state = PWM_STATE_OPEN;
            }
        }
    }
}

/*
 * @brief    This function handles TIMER15 exception
 * @param    None
 * @retval   None
 */
void TIMER15_IRQHandler(void)
{
    static uint32_t counter, wave_update, display_update, vpp_update;

    if (SET == timer_interrupt_flag_get(TIMER15, TIMER_INT_FLAG_UP)) {
        timer_interrupt_flag_clear(TIMER15, TIMER_INT_FLAG_UP);
        if (counter == wave_update) {
            wave_update = (counter + app_data.wave_samp_freq) % 0xFFFFFFFF;
            if (app_state.waveform_update == WAVEFORM_UPDATE_NONE)
                app_state.waveform_update = WAVEFORM_UPDATE_DONE;                   // the default update is 500ms
        }
        if (counter == display_update) {
            display_update = (counter + 10000) % 0xFFFFFFFF;
            app_state.display_update = APP_STATE_ENABLE;                            // the display content is updated 500ms at lowest
        }
        if (vpp_update == counter) {
            app_data.wave_vpp = -100;
            vpp_update = (counter + 500000) % 0xFFFFFFFF;
        }
    }
    counter ++;  // 10us
}

/*
 * @brief    This function handles TIMER2 exception
 * @param    None
 * @retval   None
 */
void TIMER2_IRQHandler(void)
{
    static uint32_t buf;
    EMLOG(LOG_DEBUG, "TIMER2_IRQHandler");
    if (SET == timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_CH0)) {
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_CH0);
        if (app_data.wave_freq) value = 0;
        if (!value) {
            value = timer_channel_capture_value_register_read(TIMER2, TIMER_CH_0);
            return ;
        }
        buf = timer_channel_capture_value_register_read(TIMER2, TIMER_CH_0);
        app_data.wave_freq = (0xFFFF + buf - value) % 0xFFFF;
        app_state.display_update = ENABLE;
    }
}
