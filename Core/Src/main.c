/*!
    \file    main.c
    \brief   led spark with systick, USART print and key example
    
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

#include "gd32e23x.h"
#include "systick.h"
#include "gd32e23x_it.h"
#include "app.h"
// #include <stdio.h>
// #include "main.h"
// #include "gd32e230c_eval.h"
// #include "lcd.h"

// static uint32_t adc_value;
// uint8_t arr_char[] = "Bark";
// int16_t wave_arr[160];
// uint8_t idx;

/*!
    \brief      main function
    \retval     none
*/
int main(void)
{
    /* configure systick */
    systick_config();
    AppInit();
    /* initilize the LEDs, USART and key */
    // GPIO_Init();
    // USART_Init();
    // ADC_Init();
    // Timer_Init();
    // LCD_Init();
    // LCD_Fill(0, 0, LCD_WIDTH, LCD_HEIGHT + 1, BLACK);
    // // LCD_Draw_Rectangle(32, 32, 96, 96, BLUE);
    // LCD_Draw_Line(16, 32, 64, 96, BLUE);
    // // LCD_Draw_Line(16, 80, 16, 32, RED);
    // LCD_Show_Char(32, 32, 'R', YELLOW, BLACK, 16, 0);
    // LCD_Show_String(16, 64, arr_char, WHITE, BLUE, 16, 0);
    // for (int i = 0; i < 10; i++) {
    //     wave_arr[i] = 0;
    //     wave_arr[i + 20] = 0;
    //     wave_arr[i + 40] = 0;
    //     wave_arr[i + 60] = 0;
    // }
    // for (int i = 10; i < 20; i++) {
    //     wave_arr[i] = 34;
    //     wave_arr[i + 20] = 34;
    //     wave_arr[i + 40] = 34;
    //     wave_arr[i + 60] = 34;
    // }
    // for (int i = 80; i < 90; i++) {
    //     wave_arr[i] = i - 80;
    //     wave_arr[i + 20] = i - 80;
    //     wave_arr[i + 40] = i - 80;
    //     wave_arr[i + 60] = i - 80;
    // }
    // for (int i = 90; i < 100; i++) {
    //     wave_arr[i] = i - 100;
    //     wave_arr[i + 20] = i - 100;
    //     wave_arr[i + 40] = i - 100;
    //     wave_arr[i + 60] = i - 100;
    // }
    // for (int i = 0; i < 160; i++) {
    //     LCD_Draw_Curve(64, wave_arr[i]);
    // }

    // printf("App Start!\n");

    // LCD_Show_String(32, 32, (const uint8_t *)"Thanks", 0X0000, 0XFFFF, 16, 0);
    // LCD_Show_String(64, 32, (const uint8_t *)"Welcome", 0XFFFF, 0X0000, 16, 0);
    
    /* print out the clock frequency of system, AHB, APB1 and APB2 */
    // printf("\r\nCK_SYS is %lu", rcu_clock_freq_get(CK_SYS));
    // printf("\r\nCK_AHB is %lu", rcu_clock_freq_get(CK_AHB));
    // printf("\r\nCK_APB1 is %lu", rcu_clock_freq_get(CK_APB1));
    // printf("\r\nCK_APB2 is %lu", rcu_clock_freq_get(CK_APB2));

    while(1) {
        AppStart();
        // if (RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_13)) {
        //     printf("Welcome to China!\n");
        //     gpio_bit_toggle(GPIOC, GPIO_PIN_13);
        //     while (RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_13));
        // }
        // if (RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_14)) {
        //     adc_value = ADCGetValue(ADC_CHANNEL_3);
        //     printf("ADC value: %lu\n", adc_value);
        //     printf("Valtage: %lf\n", (float)adc_value/4095*3.3f);
        //     while (RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_14));
        // }
        // if (RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_15)) {
        //     gpio_bit_toggle(GPIOC, GPIO_PIN_14|GPIO_PIN_15);
        //     while (RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_15));
        // }
        // if (RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_9)) {
        //     static uint32_t pwm_width = 5000;
        //     printf("EC11 was pressed!\n");
        //     if (pwm_width < 10000) {
        //         pwm_width += 500;
        //     } else {
        //         pwm_width = 500;
        //     }
        //     timer_channel_output_pulse_value_config(TIMER14, TIMER_CH_0, pwm_width);
        //     while (RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_9));
        // }
        // if (rotate_flag == 1) {
        //     printf("EC11 Turn Right.\n");
        // } else if (rotate_flag == 2) {
        //     printf("EC11 Turn Left.\n");
        // }
        // if (timer_flag) {
        //     if (time_value) {
        //         printf("Signal Frequency: %lf\n", (float)1000000 / time_value);
        //         time_value = 0;
        //     }
        //     // printf("Timer15 interrupt!\n");
        //     timer_flag = 0;
        // }
        // rotate_flag = 0;
    }
}
