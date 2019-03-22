/*
 * 25lc010a.c
 *
 *  Created on: 2019. 3. 22.
 *      Author: KIKI
 */

#include "25lc010a.h"

SPI_HandleTypeDef* EEPROM_SPI;
GPIO_TypeDef* EEPROM_CS_Port;
uint16_t EEPROM_CS_Pin;

static void EEPROM_Select();
static void EEPROM_Deselect();

void EEPROM_Init(SPI_HandleTypeDef* spi, GPIO_TypeDef* cs_port, uint16_t cs_pin)
{
	EEPROM_SPI = spi;
	EEPROM_CS_Port = cs_port;
	EEPROM_CS_Pin = cs_pin;
}

void EEPROM_WriteEnable(void)
{
	uint8_t data = EEPROM_CMD_WREN;
	EEPROM_Select();
	HAL_SPI_Transmit(EEPROM_SPI, &data, 1, HAL_MAX_DELAY);
	EEPROM_Deselect();
}

uint8_t EEPROM_ReadByte(uint8_t address)
{
	uint8_t ret;
	uint8_t data[2] = {EEPROM_CMD_READ, address};

	EEPROM_Select();
	HAL_SPI_Transmit(EEPROM_SPI, data, sizeof(data), HAL_MAX_DELAY);
	HAL_SPI_Receive(EEPROM_SPI, &ret, 1, HAL_MAX_DELAY);
	EEPROM_Deselect();

	return ret;
}

void EEPROM_ReadBuffer(uint8_t address, uint8_t *buffer, uint8_t length)
{
	for(uint8_t i=0; i < length; i++)
	{
		buffer[i] = EEPROM_ReadByte(address+i);
	}
}

void EEPROM_WriteByte(uint8_t address, uint8_t data)
{
	uint8_t buffer[3] = {EEPROM_CMD_WRITE, address, data};

	EEPROM_WriteEnable();
	EEPROM_Select();
	HAL_SPI_Transmit(EEPROM_SPI, buffer, sizeof(buffer), HAL_MAX_DELAY);
	EEPROM_Deselect();
	HAL_Delay(5);
}

void EEPROM_WriteBuffer(uint8_t address, uint8_t *buffer, uint8_t length)
{
	uint8_t data[2] = {EEPROM_CMD_WRITE, address};

	EEPROM_WriteEnable();
	EEPROM_Select();
	HAL_SPI_Transmit(EEPROM_SPI, data, sizeof(data), HAL_MAX_DELAY);

	for(uint8_t i = 0; i < length; i++)
	{
		if( (address+i) % EEPROM_PAGE_SIZE == 0 && i != 0 )
		{
			EEPROM_Deselect();
			HAL_Delay(5);

			EEPROM_WriteEnable();
			EEPROM_Select();
			data[1] = address + i;
			HAL_SPI_Transmit(EEPROM_SPI, data, sizeof(data), HAL_MAX_DELAY);
		}

		HAL_SPI_Transmit(EEPROM_SPI, &buffer[i], 1, HAL_MAX_DELAY);
	}

	EEPROM_Deselect();
	HAL_Delay(5);
}

void EEPROM_EraseAll(void)
{
	uint8_t address = 0;
	uint8_t data[2] = {EEPROM_CMD_WRITE, address};

	EEPROM_WriteEnable();

	while(address < EEPROM_TOTAL_SIZE)
	{
		EEPROM_Select();
		HAL_SPI_Transmit(EEPROM_SPI, data, sizeof(data), HAL_MAX_DELAY);

		for(uint8_t i=0; i < EEPROM_PAGE_SIZE; i++)
		{
			HAL_SPI_Transmit(EEPROM_SPI, &data[1], 1, HAL_MAX_DELAY);
		}

		EEPROM_Deselect();
		HAL_Delay(5);
		address += EEPROM_PAGE_SIZE;
	}
}

static void EEPROM_Select()
{
	HAL_GPIO_WritePin(EEPROM_CS_Port, EEPROM_CS_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
}

static void EEPROM_Deselect()
{
	HAL_GPIO_WritePin(EEPROM_CS_Port, EEPROM_CS_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
}