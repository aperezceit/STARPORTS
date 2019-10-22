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
/** ============================================================================
 *  @file       C3220SF_STARPORTS.h
 *
 *  @brief      CC3220 Board Specific APIs
 *
 *  The CC3220SF_STARPORTS header file should be included in an application as
 *  follows:
 *  @code
 *  #include <CC3220SF_STARPORTS.h>
 *  @endcode
 *
 *  ============================================================================
 */
#ifndef __CC3220SF_STARPORTS_H
#define __CC3220SF_STARPORTS_H

#ifdef __cplusplus
extern "C" {
#endif

#define CC3220SF_STARPORTS_GPIO_LED_OFF (0)
#define CC3220SF_STARPORTS_GPIO_LED_ON  (1)

/*!
 *  @def    CC3220SF_STARPORTS_ADCName
 *  @brief  Enum of ADC names on the CC3220SF_STARPORTS dev board
 */
typedef enum CC3220SF_STARPORTS_ADCName {
    CC3220SF_STARPORTS_ADC0 = 0,

    CC3220SF_STARPORTS_ADCCOUNT
} CC3220SF_STARPORTS_ADCName;

/*!
 *  @def    CC3220SF_STARPORTS_CaptureName
 *  @brief  Enum of Capture names on the CC3220SF_STARPORTS dev board
 */
typedef enum CC3220SF_STARPORTS_CaptureName {
    CC3220SF_STARPORTS_CAPTURE0 = 0,
    CC3220SF_STARPORTS_CAPTURE1,

    CC3220SF_STARPORTS_CAPTURECOUNT
} CC3220SF_STARPORTS_CaptureName;

/*!
 *  @def    CC3220SF_STARPORTS_CryptoName
 *  @brief  Enum of Crypto names on the CC3220SF_STARPORTS dev board
 */
typedef enum CC3220SF_STARPORTS_CryptoName {
    CC3220SF_STARPORTS_CRYPTO0 = 0,

    CC3220SF_STARPORTS_CRYPTOCOUNT
} CC3220SF_STARPORTS_CryptoName;

/*!
 *  @def    CC3220SF_STARPORTS_GPIOName
 *  @brief  Enum of GPIO names on the CC3220SF_STARPORTS dev board
 */
typedef enum CC3220SF_STARPORTS_GPIOName {
    /* /MCLR Signal for RN2483 */
    CC3220SF_STARPORTS_RN2483_MCLR,

    /* STARPORTS GPIO Pins */
    CC3220SF_STARPORTS_ADXL355_CS,
    CC3220SF_STARPORTS_BME280_CS,
    CC3220SF_STARPORTS_LDC1000_CS,
    CC3220SF_STARPORTS_EN_NODE,
    CC3220SF_STARPORTS_EN_ADXL355,
    CC3220SF_STARPORTS_EN_BME280,
    CC3220SF_STARPORTS_EN_LDC1000,
    CC3220SF_STARPORTS_DS1374_INTB,

    CC3220SF_STARPORTS_GPIOCOUNT
} CC3220SF_STARPORTS_GPIOName;

/*!
 *  @def    CC3220SF_STARPORTS_I2CName
 *  @brief  Enum of I2C names on the CC3220SF_STARPORTS dev board
 */
typedef enum CC3220SF_STARPORTS_I2CName {
    CC3220SF_STARPORTS_I2C0 = 0,

    CC3220SF_STARPORTS_I2CCOUNT
} CC3220SF_STARPORTS_I2CName;

/*!
 *  @def    CC3220SF_STARPORTS_PWMName
 *  @brief  Enum of PWM names on the CC3220SF_STARPORTS dev board
 */
typedef enum CC3220SF_STARPORTS_PWMName {
    CC3220SF_STARPORTS_PWM5 = 0,

    CC3220SF_STARPORTS_PWMCOUNT
} CC3220SF_STARPORTS_PWMName;

/*!
 *  @def    CC3220SF_STARPORTS_SDFatFSName
 *  @brief  Enum of SDFatFS names on the CC3220SF_STARPORTS dev board
 */
typedef enum CC3220SF_STARPORTS_SDFatFSName {
    CC3220SF_STARPORTS_SDFatFS0 = 0,

    CC3220SF_STARPORTS_SDFatFSCOUNT
} CC3220SF_STARPORTS_SDFatFSName;

/*!
 *  @def    CC3220SF_STARPORTS_SDName
 *  @brief  Enum of SD names on the CC3220SF_STARPORTS dev board
 */
typedef enum CC3220SF_STARPORTS_SDName {
    CC3220SF_STARPORTS_SD0 = 0,

    CC3220SF_STARPORTS_SDCOUNT
} CC3220SF_STARPORTS_SDName;

/*!
 *  @def    CC3220SF_STARPORTS_SPIName
 *  @brief  Enum of SPI names on the CC3220SF_STARPORTS dev board
 */
typedef enum CC3220SF_STARPORTS_SPIName {
    CC3220SF_STARPORTS_SPI0 = 0,
    CC3220SF_STARPORTS_SPI1,

    CC3220SF_STARPORTS_SPICOUNT
} CC3220SF_STARPORTS_SPIName;

/*!
 *  @def    CC3220SF_STARPORTS_TimerName
 *  @brief  Enum of Timer names on the CC3220SF_STARPORTS dev board
 */
typedef enum CC3220SF_STARPORTS_TimerName {
    CC3220SF_STARPORTS_TIMER0 = 0,
    CC3220SF_STARPORTS_TIMER1,
    CC3220SF_STARPORTS_TIMER2,

    CC3220SF_STARPORTS_TIMERCOUNT
} CC3220SF_STARPORTS_TimerName;

/*!
 *  @def    CC3220SF_STARPORTS_UARTName
 *  @brief  Enum of UARTs on the CC3220SF_STARPORTS dev board
 */
typedef enum CC3220SF_STARPORTS_UARTName {
    CC3220SF_STARPORTS_UART0 = 0,
    CC3220SF_STARPORTS_UART1,

    CC3220SF_STARPORTS_UARTCOUNT
} CC3220SF_STARPORTS_UARTName;

/*!
 *  @def    CC3220SF_STARPORTS_WatchdogName
 *  @brief  Enum of Watchdogs on the CC3220SF_STARPORTS dev board
 */
typedef enum CC3220SF_STARPORTS_WatchdogName {
    CC3220SF_STARPORTS_WATCHDOG0 = 0,

    CC3220SF_STARPORTS_WATCHDOGCOUNT
} CC3220SF_STARPORTS_WatchdogName;

/*!
 *  @brief  Initialize the general board specific settings
 *
 *  This function initializes the general board specific settings.
 */
extern void CC3220SF_STARPORTS_initGeneral(void);

#ifdef __cplusplus
}
#endif

#endif /* __CC3220SF_STARPORTS_H */
