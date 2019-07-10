/*
    __mp3_driver.c

-----------------------------------------------------------------------------

  This file is part of mikroSDK.

  Copyright (c) 2017, MikroElektonika - http://www.mikroe.com

  All rights reserved.

----------------------------------------------------------------------------- */

#include "__mp3_driver.h"
#include "__mp3_hal.c"

/* ------------------------------------------------------------------- MACROS */

const uint8_t _MP3_WRITE_CMD        = 0x02;
const uint8_t _MP3_READ_CMD         = 0x03;

const uint8_t _MP3_BASE_ADDR        = 0x00;
const uint8_t _MP3_MODE_ADDR        = 0x00;
const uint8_t _MP3_STATUS_ADDR      = 0x01;
const uint8_t _MP3_BASS_ADDR        = 0x02;
const uint8_t _MP3_CLOCKF_ADDR      = 0x03;
const uint8_t _MP3_DECODE_TIME_ADDR = 0x04;
const uint8_t _MP3_AUDATA_ADDR      = 0x05;
const uint8_t _MP3_WRAM_ADDR        = 0x06;
const uint8_t _MP3_WRAMADDR_ADDR    = 0x07;
const uint8_t _MP3_HDAT0_ADDR       = 0x08;
const uint8_t _MP3_HDAT1_ADDR       = 0x09;
const uint8_t _MP3_AIADDR_ADDR      = 0x0A;
const uint8_t _MP3_VOL_ADDR         = 0x0B;
const uint8_t _MP3_AICTRL0_ADDR     = 0x0C;
const uint8_t _MP3_AICTRL1_ADDR     = 0x0D;
const uint8_t _MP3_AICTRL2_ADDR     = 0x0E;
const uint8_t _MP3_AICTRL3_ADDR     = 0x0F;

/* ---------------------------------------------------------------- VARIABLES */

#ifdef   __MP3_DRV_I2C__
static uint8_t _slaveAddress;
#endif


/* -------------------------------------------- PRIVATE FUNCTION DECLARATIONS */



/* --------------------------------------------- PRIVATE FUNCTION DEFINITIONS */



/* --------------------------------------------------------- PUBLIC FUNCTIONS */

#ifdef   __MP3_DRV_SPI__

void mp3_spiDriverInit(T_MP3_P gpioObj, T_MP3_P spiObj)
{
    hal_spiMap( (T_HAL_P)spiObj );
    hal_gpioMap( (T_HAL_P)gpioObj );

    hal_gpio_csSet(1);
}

#endif
#ifdef   __MP3_DRV_I2C__

void mp3_i2cDriverInit(T_MP3_P gpioObj, T_MP3_P i2cObj, uint8_t slave)
{
    _slaveAddress = slave;
    hal_i2cMap( (T_HAL_P)i2cObj );
    hal_gpioMap( (T_HAL_P)gpioObj );

    // ... power ON
    // ... configure CHIP
}

#endif
#ifdef   __MP3_DRV_UART__

void mp3_uartDriverInit(T_MP3_P gpioObj, T_MP3_P uartObj)
{
    hal_uartMap( (T_HAL_P)uartObj );
    hal_gpioMap( (T_HAL_P)gpioObj );

    // ... power ON
    // ... configure CHIP
}

#endif


/* ----------------------------------------------------------- IMPLEMENTATION */

void mp3_init()
{
    hal_gpio_intSet( 1 );
    hal_gpio_csSet( 1 );
    hal_gpio_rstSet( 1 );
}

void mp3_reset()
{
    hal_gpio_rstSet( 0 );
    Delay_100ms();
    hal_gpio_rstSet( 1 );

    while (!hal_gpio_anGet())
    {
    }
}

uint8_t mp3_isBusy()
{
    if (hal_gpio_anGet())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void mp3_cmdWrite( uint8_t address, uint16_t input )
{
    uint8_t tmp[ 4 ];

    tmp[ 0 ] = _MP3_WRITE_CMD;
    tmp[ 1 ] = address;
    tmp[ 2 ] = input >> 8;
    tmp[ 3 ] = input & 0x00FF;

    hal_gpio_csSet( 0 );
    hal_spiWrite( tmp, 4 );
    hal_gpio_csSet( 1 );

// Poll DREQ pin and block until module is ready to receive another command
    while (!hal_gpio_anGet());
}

uint16_t mp3_cmdRead( uint8_t address )
{
    uint8_t tmp_in[ 4 ];
    uint8_t tmp_out[ 4 ];
    uint16_t res;

    tmp_in[ 0 ] = _MP3_READ_CMD;
    tmp_in[ 1 ] = address;
    tmp_in[ 2 ] = 0;
    tmp_in[ 3 ] = 0;


    hal_gpio_csSet( 0 );
    hal_spiTransfer( tmp_in, tmp_out, 4 );
    hal_gpio_csSet( 1 );

    res = tmp_out[3];
    res <<= 8;
    res |= tmp_out[4];

// Poll DREQ pin and block until module is ready to receive another command
    while (!hal_gpio_anGet());

    return res;
}

uint8_t mp3_dataWrite( uint8_t input )
{
    uint8_t tmp;

    if (!hal_gpio_anGet())
    {
        return 1;
    }

    tmp = input;

    hal_gpio_intSet( 0 );
    hal_spiWrite( &tmp, 1 );
    hal_gpio_intSet( 1 );

    return 0;
}

uint8_t mp3_dataWrite32( uint8_t *input32 )
{
    if (!hal_gpio_anGet())
    {
        return 1;
    }

    hal_gpio_intSet( 0 );
    hal_spiWrite( input32, 32 );
    hal_gpio_intSet( 1 );

    return 0;
}

/* -------------------------------------------------------------------------- */
/*
  __mp3_driver.c

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