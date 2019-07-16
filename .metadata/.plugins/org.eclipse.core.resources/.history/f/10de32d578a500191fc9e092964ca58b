#include "vs1053.h"

/* Commands */
#define VS1053_WRITE_CMD	0x02;
#define VS1053_READ_CMD		0x03;

/* Registers */
const uint8_t VS1053_REG_BASE		= 0x00;
const uint8_t VS1053_REG_MODE   	= 0x00;
const uint8_t VS1053_REG_STATUS 	= 0x01;
const uint8_t VS1053_REG_BASS 		= 0x02;
const uint8_t VS1053_REG_CLOCKF 	= 0x03;
const uint8_t VS1053_REG_DECODE_TIME = 0x04;
const uint8_t VS1053_REG_AUDATA 	= 0x05;
const uint8_t VS1053_REG_WRAM 		= 0x06;
const uint8_t VS1053_REG_WRAMADDR 	= 0x07;
const uint8_t VS1053_REG_HDAT0 		= 0x08;
const uint8_t VS1053_REG_HDAT1 		= 0x09;
const uint8_t VS1053_REG_AIADDR 	= 0x0A;
const uint8_t VS1053_REG_VOL 		= 0x0B;
const uint8_t VS1053_REG_AICTRL0 	= 0x0C;
const uint8_t VS1053_REG_AICTRL1 	= 0x0D;
const uint8_t VS1053_REG_AICTRL2 	= 0x0E;
const uint8_t VS1053_REG_AICTRL3 	= 0x0F;

/* Pin control */
#define XCS_HIGH	HAL_GPIO_WritePin(VS1053_XCS_PORT, VS1053_XCS_PIN, GPIO_PIN_SET)
#define XCS_LOW		HAL_GPIO_WritePin(VS1053_XCS_PORT, VS1053_XCS_PIN, GPIO_PIN_RESET)
#define XDCS_HIGH	HAL_GPIO_WritePin(VS1053_XDCS_PORT, VS1053_XDCS_PIN, GPIO_PIN_SET)
#define XDCS_LOW	HAL_GPIO_WritePin(VS1053_XDCS_PORT, VS1053_XDCS_PIN, GPIO_PIN_RESET)
#define XRST_HIGH	HAL_GPIO_WritePin(VS1053_XRST_PORT, VS1053_XRST_PIN, GPIO_PIN_SET)
#define XRST_LOW	HAL_GPIO_WritePin(VS1053_XRST_PORT, VS1053_XRST_PIN, GPIO_PIN_RESET)

/* endFill byte is required to stop playing */
uint8_t endFillByte;

/* Initialize VS1053 */
bool VS1053_Init()
{
	uint16_t status = 0;

	XCS_HIGH;		    /* XCS High */
	XDCS_HIGH;		    /* XDCS High */
	VS1053_Reset();     /* Hard Reset */

	/* x 1.0 Clock, 12MHz / 7, SPI Baudrate should be less than 1.75MHz */
	(HSPI_VS1053)->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;  /* 42MHz / 32 = 1.31MHz */
	if(HAL_SPI_Init(HSPI_VS1053) != HAL_OK) return false;

	/* Read Status to check SPI */
	if(!VS1053_SciRead(VS1053_REG_STATUS, &status)) return false;
	if(((status >> 4) & 0x0F) != 0x04) return false;

	/* MP3 Mode GPIO configuration */
	if(!VS1053_SciWrite(VS1053_REG_WRAMADDR, 0xC017)) return false; /* GPIO direction */
	if(!VS1053_SciWrite(VS1053_REG_WRAM, 3)) return false;
	if(!VS1053_SciWrite(VS1053_REG_WRAMADDR, 0xC019)) return false; /* GPIO output */
	if(!VS1053_SciWrite(VS1053_REG_WRAM, 0)) return false;

	/* Soft reset */
	if(!VS1053_SoftReset()) return false;

	/* x3.0 Clock, 36MHz / 7, SPI Baudrate should be less than 5.14MHz */
	if(!VS1053_SciWrite(VS1053_REG_CLOCKF, 0x6000)) return false;

	(HSPI_VS1053)->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;  /* 42MHz / 16 = 2.625MHz */
	if(HAL_SPI_Init(HSPI_VS1053) != HAL_OK) return false;

	/* Read Status to check SPI */
	if(!VS1053_SciRead(VS1053_REG_STATUS, &status)) return false;
	if(((status >> 4) & 0x0F) != 0x04) return false;

	/* Read endFill Byte */
	uint16_t regVal;
	if(!VS1053_SciWrite(VS1053_REG_WRAMADDR, 0x1E06)) return false;	/* endFill */
	if(!VS1053_SciRead(VS1053_REG_WRAM, &regVal)) return false;
	endFillByte = regVal & 0xFF;

	return true;
}

/* Hard reset */
void VS1053_Reset()
{
	uint8_t dummy = 0xFF;
	XRST_LOW;		                                    /* XRST Low */
	HAL_SPI_Transmit(HSPI_VS1053, &dummy, 1, 10);       /* Tx Dummy */
	HAL_Delay(10);										/* 10ms Delay */
	XRST_HIGH;			                                /* XRST High */
	HAL_Delay(10);
}

/* Soft reset */
bool VS1053_SoftReset()
{
	if(!VS1053_SciWrite(VS1053_REG_MODE, 0x4804)) return false;	/* SM LINE1 | SM SDINEW | SM RESET */
	HAL_Delay(100);
	return true;
}

/* Volume control */
bool VS1053_SetVolume(uint8_t volumeLeft, uint8_t volumeRight)
{
    uint16_t volume;
    volume = ( volumeLeft << 8 ) + volumeRight;

    if(!VS1053_SciWrite(VS1053_REG_VOL, volume)) return false;
    return true;
}

/* Mode control */
bool VS1053_SetMode(uint16_t mode)
{
	if(!VS1053_SciWrite(VS1053_REG_MODE, mode)) return false;
	return true;
}

bool VS1053_GetMode(uint16_t *mode)
{
	if(!VS1053_SciRead(VS1053_REG_MODE, mode)) return false;
	return true;
}

/* Resync control */
bool VS1053_AutoResync()
{
	if(!VS1053_SciWrite(VS1053_REG_WRAMADDR, 0x1E29)) return false; /* Auto Resync */
	if(!VS1053_SciWrite(VS1053_REG_WRAM, 0)) return false;
	return true;
}

/* Set decode time */
bool VS1053_SetDecodeTime(uint16_t time)
{
	if(!VS1053_SciWrite(VS1053_REG_DECODE_TIME, time)) return false;
	if(!VS1053_SciWrite(VS1053_REG_DECODE_TIME, time)) return false;
	return true;
}

/* Send endfill bytes */
bool VS1053_SendEndFill(uint16_t num)
{
	for(uint16_t i = 0; i < num; i++)
	{
		VS1053_SdiWrite(endFillByte);
	}
	return true;
}

/* Check DREQ pin */
bool VS1053_IsBusy()
{
	if (HAL_GPIO_ReadPin(VS1053_DREQ_PORT, VS1053_DREQ_PIN) == GPIO_PIN_SET) return false;  /* Ready */
	else return true;   /* Busy */
}

/* SCI Tx */
bool VS1053_SciWrite( uint8_t address, uint16_t input )
{
	uint8_t buffer[4];

	buffer[0] = VS1053_WRITE_CMD;
	buffer[1] = address;
	buffer[2] = input >> 8;			/* Input MSB */
	buffer[3] = input & 0x00FF;		/* Input LSB */

	while (HAL_GPIO_ReadPin(VS1053_DREQ_PORT, VS1053_DREQ_PIN) == GPIO_PIN_RESET);	/* Wait DREQ High */

	XCS_LOW;			/* XCS Low */
	if(HAL_SPI_Transmit(HSPI_VS1053, buffer, sizeof(buffer), 10) != HAL_OK) return false;
	XCS_HIGH;			/* XCS High */

	while (HAL_GPIO_ReadPin(VS1053_DREQ_PORT, VS1053_DREQ_PIN) == GPIO_PIN_RESET);	/* Wait DREQ High */
	return true;
}

/* SCI TxRx */
bool VS1053_SciRead( uint8_t address, uint16_t *res)
{
	uint8_t dummy = 0xFF;
	uint8_t txBuffer[2];
	uint8_t rxBuffer[2];

	txBuffer[0] = VS1053_READ_CMD;
	txBuffer[1] = address;

	while(HAL_GPIO_ReadPin(VS1053_DREQ_PORT, VS1053_DREQ_PIN) == GPIO_PIN_RESET);	/* Wait DREQ High */

	XCS_LOW;        /* XCS Low */
	if(HAL_SPI_Transmit(HSPI_VS1053, txBuffer, sizeof(txBuffer), 10) != HAL_OK) return false;
	if(HAL_SPI_TransmitReceive(HSPI_VS1053, &dummy, &rxBuffer[0], 1, 10) != HAL_OK) return false;
	if(HAL_SPI_TransmitReceive(HSPI_VS1053, &dummy, &rxBuffer[1], 1, 10) != HAL_OK) return false;
	XCS_HIGH;       /* XCS High */

	*res = rxBuffer[0];     /* Received data */
	*res <<= 8;				/* MSB */
	*res |= rxBuffer[1];	/* LSB */

	while (HAL_GPIO_ReadPin(VS1053_DREQ_PORT, VS1053_DREQ_PIN) == GPIO_PIN_RESET);	/* Wait DREQ High */
	return true;
}

/* SDI Tx */
bool VS1053_SdiWrite( uint8_t input )
{
	while (HAL_GPIO_ReadPin(VS1053_DREQ_PORT, VS1053_DREQ_PIN) == GPIO_PIN_RESET);	/* Wait DREQ High */

	XDCS_LOW;			/* XDCS Low(SDI) */
	if(HAL_SPI_Transmit(HSPI_VS1053, &input, 1, 10) != HAL_OK) return false;		/* SPI Tx 1 byte */
	XDCS_HIGH;			/* XDCS High(SDI) */

	return true;
}

/* SDI Tx 32 bytes */
bool VS1053_SdiWrite32( uint8_t *input32 )
{
	while (HAL_GPIO_ReadPin(VS1053_DREQ_PORT, VS1053_DREQ_PIN) == GPIO_PIN_RESET);	/* Wait DREQ High */

	XDCS_LOW;			/* XDCS Low(SDI) */
	if(HAL_SPI_Transmit(HSPI_VS1053, input32, 32, 10) != HAL_OK) return false;		/* SPI Tx 32 bytes */
	XDCS_HIGH;			/* XDCS High(SDI) */

	return true;
}
