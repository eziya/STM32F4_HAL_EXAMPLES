/*
    __mp3_driver.h

-----------------------------------------------------------------------------

  This file is part of mikroSDK.
  
  Copyright (c) 2017, MikroElektonika - http://www.mikroe.com

  All rights reserved.

----------------------------------------------------------------------------- */

/**
@file   __mp3_driver.h
@brief    MP3 Driver
@mainpage MP3 Click
@{

@image html libstock_fb_view.jpg

@}

@defgroup   MP3
@brief      MP3 Click Driver
@{

| Global Library Prefix | **MP3** |
|:---------------------:|:-----------------:|
| Version               | **1.0.0**    |
| Date                  | **apr 2018.**      |
| Developer             | **MikroE Team**     |

*/
/* -------------------------------------------------------------------------- */

#include "stdint.h"

#ifndef _MP3_H_
#define _MP3_H_

/** 
 * @macro T_MP3_P
 * @brief Driver Abstract type 
 */
#define T_MP3_P    const uint8_t*

/** @defgroup MP3_COMPILE Compilation Config */              /** @{ */

   #define   __MP3_DRV_SPI__                            /**<     @macro __MP3_DRV_SPI__  @brief SPI driver selector */
//  #define   __MP3_DRV_I2C__                            /**<     @macro __MP3_DRV_I2C__  @brief I2C driver selector */                                          
// #define   __MP3_DRV_UART__                           /**<     @macro __MP3_DRV_UART__ @brief UART driver selector */ 

                                                                       /** @} */
/** @defgroup MP3_VAR Variables */                           /** @{ */

extern const uint8_t _MP3_WRITE_CMD;
extern const uint8_t _MP3_READ_CMD;

extern const uint8_t _MP3_BASE_ADDR;
extern const uint8_t _MP3_MODE_ADDR;
extern const uint8_t _MP3_STATUS_ADDR;
extern const uint8_t _MP3_BASS_ADDR;
extern const uint8_t _MP3_CLOCKF_ADDR;
extern const uint8_t _MP3_DECODE_TIME_ADDR ;
extern const uint8_t _MP3_AUDATA_ADDR;
extern const uint8_t _MP3_WRAM_ADDR;
extern const uint8_t _MP3_WRAMADDR_ADDR;
extern const uint8_t _MP3_HDAT0_ADDR;
extern const uint8_t _MP3_HDAT1_ADDR;
extern const uint8_t _MP3_AIADDR_ADDR;
extern const uint8_t _MP3_VOL_ADDR;
extern const uint8_t _MP3_AICTRL0_ADDR;
extern const uint8_t _MP3_AICTRL1_ADDR;
extern const uint8_t _MP3_AICTRL2_ADDR;
extern const uint8_t _MP3_AICTRL3_ADDR;

                                                                       /** @} */
#ifdef __cplusplus
extern "C"{
#endif

/** @defgroup MP3_INIT Driver Initialization */              /** @{ */

#ifdef   __MP3_DRV_SPI__
void mp3_spiDriverInit(T_MP3_P gpioObj, T_MP3_P spiObj);
#endif
#ifdef   __MP3_DRV_I2C__
void mp3_i2cDriverInit(T_MP3_P gpioObj, T_MP3_P i2cObj, uint8_t slave);
#endif
#ifdef   __MP3_DRV_UART__
void mp3_uartDriverInit(T_MP3_P gpioObj, T_MP3_P uartObj);
#endif

// GPIO Only Drivers - remove in other cases
void mp3_gpioDriverInit(T_MP3_P gpioObj);
                                                                       /** @} */
/** @defgroup MP3_FUNC Driver Functions */                   /** @{ */

/**
 * @brief Functions for initialize MP3 module
 */
void mp3_init();

/**
 * @brief Functions for reset MP3 module
 */
void mp3_reset();

/**
 * @brief Function that checks whether busy mp3
 *
 * @retvale  1 - is Busy / 0 - is not busy
 */
uint8_t mp3_isBusy();

/**
 * @brief  Function writes one byte (command) to MP3
 *
 * @param[in] address   adddress of register whitch be written
 * @param[in] input     command which be written
 */
void mp3_cmdWrite( uint8_t address, uint16_t input );

/**
 * @brief  Function reads command from MP3
 *
 * @retval command that is read
 */
uint16_t mp3_cmdRead( uint8_t address );

/**
 * @brief  Function writes one byte ( data ) to MP3
 *
 * @param[in] input     data which be written
 */
uint8_t mp3_dataWrite( uint8_t input );

/**
 * @brief  Function Write 32 bytes ( data ) to MP3
 *
 * @param[in] input32     buffer of 32 bytes which be written
 */
uint8_t mp3_dataWrite32( uint8_t *input32 );





                                                                       /** @} */
#ifdef __cplusplus
} // extern "C"
#endif
#endif

/**
    @example Click_MP3_STM.c
    @example Click_MP3_TIVA.c
    @example Click_MP3_CEC.c
    @example Click_MP3_KINETIS.c
    @example Click_MP3_MSP.c
    @example Click_MP3_PIC.c
    @example Click_MP3_PIC32.c
    @example Click_MP3_DSPIC.c
    @example Click_MP3_AVR.c
    @example Click_MP3_FT90x.c
    @example Click_MP3_STM.mbas
    @example Click_MP3_TIVA.mbas
    @example Click_MP3_CEC.mbas
    @example Click_MP3_KINETIS.mbas
    @example Click_MP3_MSP.mbas
    @example Click_MP3_PIC.mbas
    @example Click_MP3_PIC32.mbas
    @example Click_MP3_DSPIC.mbas
    @example Click_MP3_AVR.mbas
    @example Click_MP3_FT90x.mbas
    @example Click_MP3_STM.mpas
    @example Click_MP3_TIVA.mpas
    @example Click_MP3_CEC.mpas
    @example Click_MP3_KINETIS.mpas
    @example Click_MP3_MSP.mpas
    @example Click_MP3_PIC.mpas
    @example Click_MP3_PIC32.mpas
    @example Click_MP3_DSPIC.mpas
    @example Click_MP3_AVR.mpas
    @example Click_MP3_FT90x.mpas
*/                                                                     /** @} */
/* -------------------------------------------------------------------------- */
/*
  __mp3_driver.h

  Copyright (c) 2017, MikroElektonika - http://www.mikroe.com

  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the MikroElektonika.

4. Neither the name of the MikroElektonika nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY MIKROELEKTRONIKA ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL MIKROELEKTRONIKA BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------------- */