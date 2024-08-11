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
#include "main.h"
#include "systick.h"

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
        delay_1ms(3); // 延时去抖动
        if (RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_3)) {
            rotate_flag = 1;
        } else {
            rotate_flag = 2;
        }
        exti_interrupt_flag_clear(EXTI_4);
    }
}

/*
 * @brief    This function handles TIMER15 exception
 * @param    None
 * @retval   None
 */
void TIMER15_IRQHandler(void)
{
    if (SET == timer_interrupt_flag_get(TIMER15, TIMER_INT_UP)) {
        if (!timer_flag) { 
            timer_flag = 1;
        }
        timer_interrupt_flag_clear(TIMER15, TIMER_INT_UP);
    }
}

/*
 * @brief    This function handles TIMER2 exception
 * @param    None
 * @retval   None
 */
void TIMER2_IRQHandler(void)
{
    if (SET == timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_CH0)) {
        uint32_t buf;
        if (time_value) value = 0;
        if (!value) {
            value = timer_channel_capture_value_register_read(TIMER2, TIMER_CH_0);
            return ;
        }
        buf = timer_channel_capture_value_register_read(TIMER2, TIMER_CH_0);
        time_value = (0xFFFF + buf - value) % 0xFFFF;
        value = buf;
    }
}