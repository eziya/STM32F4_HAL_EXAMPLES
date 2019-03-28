/*
 * max7219.c
 *
 *  Created on: 2019. 3. 22.
 *      Author: KIKI
 */


#include "MAX7219.h"

SPI_HandleTypeDef* Max7219_SPI;
GPIO_TypeDef* Max7219_SS_Port;
uint16_t Max7219_SS_Pin;

static void SS_Select();
static void SS_Deselect();
static bool SPI_Tx(uint8_t data);
static void DelayInit(void);
static void DelayUS(uint32_t);

void MAX7219_Init(SPI_HandleTypeDef* spi, GPIO_TypeDef* ss_port, uint16_t ss_pin)
{
	Max7219_SPI = spi;
	Max7219_SS_Port = ss_port;
	Max7219_SS_Pin = ss_pin;

	/* Us delay */
	DelayInit();

	/* Deselect SS */
	SS_Deselect();
}

bool MAX7219_Write(uint8_t index, uint8_t reg, uint8_t data)
{
	if(index >= MAX7219_IC_NUM) return false;

	SS_Select();

	/* NOOP to following ic */
	for(int i = index; i < MAX7219_IC_NUM-1; i++)
	{
		if(!SPI_Tx(MAX7219_REG_NOOP)) return false;		/* Reg */
		if(!SPI_Tx(MAX7219_REG_NOOP)) return false;		/* Data */
	}

	/* Write register */
	if(!SPI_Tx(reg)) return false;
	if(!SPI_Tx(data)) return false;

	/* NOOP to previous ic */
	for(int i = 0; i < index; i++)
	{
		if(!SPI_Tx(MAX7219_REG_NOOP)) return false;		/* Reg */
		if(!SPI_Tx(MAX7219_REG_NOOP)) return false;		/* Data */
	}

	SS_Deselect();

	return true;
}

bool MAX7219_Digit(uint8_t index, uint8_t digit, int8_t value)
{
	if(index >= MAX7219_IC_NUM) return false;
	if(digit > 0x07) return false;

	if(!MAX7219_Write(index, digit+1, value)) return false;

	return true;
}

bool MAX7219_Decode(uint8_t index, uint8_t value)
{
	if(index >= MAX7219_IC_NUM) return false;

	if(!MAX7219_Write(index, MAX7219_REG_DECODE, value)) return false;

	return true;
}

bool MAX7219_Intensity(uint8_t index, uint8_t value)
{
	if(index >= MAX7219_IC_NUM) return false;
	if(value > 0x0F) value = 0x0F;

	if(!MAX7219_Write(index, MAX7219_REG_INTENSITY, value)) return false;

	return true;
}

bool MAX7219_ScanLimit(uint8_t index, uint8_t value)
{
	if(index >= MAX7219_IC_NUM) return false;
	if(value > 0x07) value = 0x07;

	if(!MAX7219_Write(index, MAX7219_REG_SCANLIMIT, value)) return false;

	return true;
}

bool MAX7219_ShutDown(uint8_t index, uint8_t value)
{
	if(index >= MAX7219_IC_NUM) return false;
	if(value > 0x01) value = 0x01;

	if(!MAX7219_Write(index, MAX7219_REG_SHUTDOWN, value)) return false;

	return true;
}

bool MAX7219_Test(uint8_t index, uint8_t value)
{
	if(index >= MAX7219_IC_NUM) return false;
	if(value > 0x01) value = 0x01;

	if(!MAX7219_Write(index, MAX7219_REG_TEST, value)) return false;

	return true;
}

static void SS_Select()
{
	HAL_GPIO_WritePin(Max7219_SS_Port, Max7219_SS_Pin, GPIO_PIN_RESET);
	DelayUS(1);
}

static void SS_Deselect()
{
	HAL_GPIO_WritePin(Max7219_SS_Port, Max7219_SS_Pin, GPIO_PIN_SET);
	DelayUS(1);
}

static bool SPI_Tx(uint8_t data)
{
	if(HAL_SPI_Transmit(Max7219_SPI, &data, 1, HAL_MAX_DELAY) != HAL_OK) return false;
	return true;
}

static void DelayInit(void)
{
  CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
  CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk;

  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
  DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;

  DWT->CYCCNT = 0;

  /* 3 NO OPERATION instructions */
  __ASM volatile ("NOP");
  __ASM volatile ("NOP");
  __ASM volatile ("NOP");
}

static void DelayUS(uint32_t us) {
  uint32_t cycles = (SystemCoreClock/1000000L)*us;
  uint32_t start = DWT->CYCCNT;
  volatile uint32_t cnt;

  do
  {
    cnt = DWT->CYCCNT - start;
  } while(cnt < cycles);
}
