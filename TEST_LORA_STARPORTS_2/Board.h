/*
 * Copyright (c) 2016-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __BOARD_H
#define __BOARD_H

#define Board_CC3220SF_STARPORTS

#ifdef __cplusplus
extern "C" {
#endif

#include "CC3220SF_STARPORTS.h"

#define Board_init                   CC3220SF_STARPORTS_initGeneral
#define Board_initGeneral            CC3220SF_STARPORTS_initGeneral

#define Board_ADC0                   CC3220SF_STARPORTS_ADC0

#define Board_CAPTURE0               CC3220SF_STARPORTS_CAPTURE0
#define Board_CAPTURE1               CC3220SF_STARPORTS_CAPTURE1

#define Board_CRYPTO0                CC3220SF_STARPORTS_CRYPTO0

#define Board_GPIO_LED_ON            CC3220SF_STARPORTS_GPIO_LED_ON
#define Board_GPIO_LED_OFF           CC3220SF_STARPORTS_GPIO_LED_OFF
#define Board_GPIO_LED0              CC3220SF_STARPORTS_GPIO_LED_D10
/*
 *  CC3220SF_STARPORTS_GPIO_LED_D8 and CC3220SF_STARPORTS_GPIO_LED_D9 are shared with the I2C
 *  and PWM peripherals. In order for those examples to work, these LEDs are
 *  taken out of gpioPinCOnfig[]
 */

#define Board_ADXL355_CS             CC3220SF_STARPORTS_ADXL355_CS
#define Board_BME280_CS              CC3220SF_STARPORTS_BME280_CS
#define Board_LDC1000_CS             CC3220SF_STARPORTS_LDC1000_CS
#define Board_EN_NODE                CC3220SF_STARPORTS_EN_NODE
#define Board_EN_ADXL355             CC3220SF_STARPORTS_EN_ADXL355
#define Board_EN_BME280              CC3220SF_STARPORTS_EN_BME280
#define Board_EN_LDC1000             CC3220SF_STARPORTS_EN_LDC1000
#define Board_DS1374_INTB            CC3220SF_STARPORTS_DS1374_INTB
#define Board_SCL_ASGPIO             CC3220SF_STARPORTS_SCL
#define Board_SDA_ASGPIO             CC3220SF_STARPORTS_SDA
#define Board_SCLK_ASGPIO            CC3220SF_STARPORTS_SCLK
#define Board_MOSI_ASGPIO            CC3220SF_STARPORTS_MOSI
#define Board_CS_ASGPIO              CC3220SF_STARPORTS_CS



#define Board_RN2483_MCLR            CC3220SF_STARPORTS_RN2483_MCLR

#define Board_I2C0                   CC3220SF_STARPORTS_I2C0
#define Board_I2C_TMP                CC3220SF_STARPORTS_I2C0

#define Board_I2S0                   CC3220SF_STARPORTS_I2S0

#define Board_PWM0                   CC3220SF_STARPORTS_PWM5
#define Board_PWM1                   CC3220SF_STARPORTS_PWM7

#define Board_SD0                    CC3220SF_STARPORTS_SD0

#define Board_SDFatFS0               CC3220SF_STARPORTS_SD0

/* CC3220SF_STARPORTS_SPI0 is reserved for the NWP */
#define Board_SPI0                   CC3220SF_STARPORTS_SPI1
#define Board_SPI_MASTER             CC3220SF_STARPORTS_SPI1
#define Board_SPI_SLAVE              CC3220SF_STARPORTS_SPI1
#define Board_SPI_MASTER_READY       CC3220SF_STARPORTS_SPI_MASTER_READY
#define Board_SPI_SLAVE_READY        CC3220SF_STARPORTS_SPI_SLAVE_READY

#define Board_TIMER0                 CC3220SF_STARPORTS_TIMER0
#define Board_TIMER1                 CC3220SF_STARPORTS_TIMER1
#define Board_TIMER2                 CC3220SF_STARPORTS_TIMER2

#define Board_UART0                  CC3220SF_STARPORTS_UART0
#define Board_UART1                  CC3220SF_STARPORTS_UART1

#define Board_WATCHDOG0              CC3220SF_STARPORTS_WATCHDOG0

/* Board specific I2C address */
#define Board_TMP006_ADDR            (0x41)
#define Board_I2C_TMP006_ADDR        Board_TMP006_ADDR

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */
