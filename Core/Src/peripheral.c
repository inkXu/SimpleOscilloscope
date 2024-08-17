#include "peripheral.h"
#include "log.h"
#include "gd32e23x.h"
#include "systick.h"
#include "stdint.h"
#include "app.h"

/*
 * @brief:  Clock configuration
 * @param:  None
 * @retval: None
 */
void RCUConfig(void)
{
	/* Enable GPIO clock */
	// rcu_periph_clock_enable(RCU_GPIOC);     // led
	rcu_periph_clock_enable(RCU_GPIOB);        // button, lcd control signal(RES, DC, CS, BLK)
	rcu_periph_clock_enable(RCU_GPIOA);        // lcd data signal(SDA, SCL), TIMER14 clock(PWM), TIMER2(FREQ), ADC, usart

    /* enable the peripheral clock */
    rcu_periph_clock_enable(RCU_CFGCMP);

    /* enable the ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    rcu_adc_clock_config(RCU_ADCCK_AHB_DIV9);  // configure the ADC frequency

    /* enable the timer clock */
    rcu_periph_clock_enable(RCU_TIMER15);      // timer
    rcu_periph_clock_enable(RCU_TIMER2);       // FREQ
    // rcu_periph_clock_enable(RCU_TIMER14);      // PWM

    /* enable the SPI clock */
    rcu_periph_clock_enable(RCU_SPI0);

#if OPEN_ZLOG == 1
    /* enable the USART clock */
    rcu_periph_clock_enable(RCU_USART0);
#endif
}

/*
 * @brief:  GPIO configuration
 * @param:  None
 * @retval: None
 */
void GPIOConfig(void)
{
    /* GPIO mode configuration */
    // LED
    gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
    // button
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_9|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
    // ADC
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_3);
    // TIMER2 PIN used to get frequency
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    // TIIMER14 PIN used to output PWM
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    // LCD control signal(RES, DC, CS, BLK)
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8);
    gpio_bit_set(GPIOB, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8);
    // LCD data signal(SDA, SCL)
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5|GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_5|GPIO_PIN_7);
    // USART
#if OPEN_ZLOG == 1
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9|GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9|GPIO_PIN_10);
#endif
    
    /* alternate function configuration */
    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_2);                      // TIMER14
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_6);                      // TIMER2
    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_5|GPIO_PIN_7);           // SPI0(SDA, SCL)
#if OPEN_ZLOG == 1
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9|GPIO_PIN_10);          // USART0
#endif

    /* enable the NVIC and configure the priority */
    nvic_irq_enable(EXTI4_15_IRQn, 1U);                             // enable key interrupt
    /* connect key EXTI line to key GPIO pin */
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN4);   // KEYA
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN15);  // KEY03
    /* configure trigger mode */
    exti_init(EXTI_4, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_init(EXTI_15, EXTI_INTERRUPT, EXTI_TRIG_FALLING);

    exti_interrupt_enable(EXTI_4);
    exti_interrupt_enable(EXTI_15);

    exti_interrupt_flag_clear(EXTI_4);
    exti_interrupt_flag_clear(EXTI_15);
}

void GPIO13_14InterruptEnable(void)
{
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN13);  // KEY01
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN14);  // KEY02
    
    exti_init(EXTI_13, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_init(EXTI_14, EXTI_INTERRUPT, EXTI_TRIG_FALLING);

    exti_interrupt_enable(EXTI_13);
    exti_interrupt_enable(EXTI_14);

    exti_interrupt_flag_clear(EXTI_13);
    exti_interrupt_flag_clear(EXTI_14);
}

/*
 * @brief:  TIMER14 configuration
 * @param:  None
 * @retval: None
 */
void Timer14Config()
{
    timer_parameter_struct timer_initpara;
    timer_oc_parameter_struct timer_ocinitpara;

    rcu_periph_clock_enable(RCU_TIMER14);

    timer_deinit(TIMER14);

    /* initialize timer 14 */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler = 720 - 1;
    timer_initpara.period = (uint32_t)50 - 1;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER14, &timer_initpara);

    timer_channel_output_struct_para_init(&timer_ocinitpara);
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER14, TIMER_CH_0, &timer_ocinitpara);

    timer_channel_output_pulse_value_config(TIMER14, TIMER_CH_0, 0);
    /* PWM0: vaild to invaild level, PWM1: invaild to vaild level */
    timer_channel_output_mode_config(TIMER14, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER14, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
    timer_auto_reload_shadow_enable(TIMER14);
    timer_counter_value_config(TIMER14 , 0);
    timer_primary_output_config(TIMER14, ENABLE);
    
    timer_enable(TIMER14);
}

/*
 * @brief:  TIMER15, TIMER2 configuration
 * @param:  None
 * @retval: None
 */
void TIMConfig(void)
{
        // 定时器初始化参数结构体
    timer_parameter_struct timer_initpara;
    timer_ic_parameter_struct timer_icinitpara;

    /* enable timer interrupt */
    nvic_irq_enable(TIMER15_IRQn, 2U);
    nvic_irq_enable(TIMER2_IRQn, 2U);

    // reset timer configuration
    timer_deinit(TIMER15);
    timer_deinit(TIMER2);
    
    // initialize timer 15
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 7200 - 1;
    timer_initpara.period            = 5000 - 1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER15, &timer_initpara);
    // initialize timer 2
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 72-1;
    timer_initpara.period            = 65535;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_init(TIMER2, &timer_initpara);
    
    // configuration timer2 input capture channel
    timer_input_capture_config(TIMER2, TIMER_CH_0, &timer_icinitpara);
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_RISING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0x0;
    timer_input_capture_config(TIMER2, TIMER_CH_0, &timer_icinitpara);

    timer_auto_reload_shadow_enable(TIMER2);
    
    timer_interrupt_flag_clear(TIMER15, TIMER_INT_FLAG_UP);
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_CH0);

    timer_interrupt_enable(TIMER15, TIMER_INT_UP);
    timer_interrupt_enable(TIMER2, TIMER_INT_CH0);

    timer_enable(TIMER15);
    timer_enable(TIMER2);
}

/*
 * @brief:  ADC configuration
 * @param:  None
 * @retval: None
 */
void ADCConfig(void)
{
    /* disable the scan mode and continuous mode, use the single conversion mode */
    adc_special_function_config(ADC_SCAN_MODE, DISABLE);
    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);
    adc_special_function_config(ADC_INSERTED_CHANNEL_AUTO, DISABLE);

    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1U);
    adc_regular_channel_config(0U, ADC_CHANNEL_3, ADC_SAMPLETIME_239POINT5);

    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);


    adc_enable();
    delay_1ms(1U);

    adc_calibration_enable();

    adc_dma_mode_enable();

    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
}

/*
 * @brief:  DMA configuration
 * @param:  None
 * @retval: None
 */
void DMAConfig(void)
{
    dma_parameter_struct dma_initpara;

    rcu_periph_clock_enable(RCU_DMA);

    nvic_irq_enable(DMA_Channel0_IRQn, 0U);

    dma_deinit(DMA_CH0);
    
    dma_struct_para_init(&dma_initpara);
    
    dma_initpara.periph_addr  = (uint32_t)(&ADC_RDATA);
    dma_initpara.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_initpara.memory_addr  = (uint32_t)(app_data.data);
    dma_initpara.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_initpara.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_initpara.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_initpara.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_initpara.number       = LENGTH;
    dma_initpara.priority     = DMA_PRIORITY_HIGH;
    
    dma_init(DMA_CH0, &dma_initpara);
    
    dma_circulation_enable(DMA_CH0);
    
    dma_channel_enable(DMA_CH0);
    
    dma_interrupt_enable(DMA_CH0, DMA_CHXCTL_FTFIE);
}

/*
 * @brief:  SPI configuration
 * @param:  None
 * @retval: None
 */
void SPIConfig(void)
{
    spi_parameter_struct spi_init_struct;

    spi_i2s_deinit(SPI0);
    spi_struct_para_init(&spi_init_struct);
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_2;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);
    spi_enable(SPI0);
}

#if OPEN_ZLOG == 1
/*
 * @brief    USART configuration
 * @param    None
 * @retval   None
 */
void USARTConfig(void)
{
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
#endif
