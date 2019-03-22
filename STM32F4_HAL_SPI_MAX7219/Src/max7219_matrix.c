/*
 * max7219_matrix.c
 *
 *  Created on: 2019. 3. 22.
 *      Author: KIKI
 */


#include "max7219_matrix.h"
#include "max7219.h"
#include <string.h>

volatile uint8_t FrameBuffer[MAX7219_IC_NUM][8];

void MAX7219_MatrixInit(SPI_HandleTypeDef* spi, GPIO_TypeDef* cs_port, uint16_t cs_pin)
{
	MAX7219_Init(spi, cs_port, cs_pin);
	for(int i = 0; i < MAX7219_IC_NUM; i++)
	{
		MAX7219_MatrixClear(i);
		MAX7219_ShutDown(i, 1);
		MAX7219_Test(i, 0);
		MAX7219_Decode(i, 0);
		MAX7219_Intensity(i, 1);
		MAX7219_ScanLimit(i, 7);
		MAX7219_MatrixClear(i);
	}
}

void MAX7219_MatrixClear(uint8_t index)
{
	for(int i =0; i < 8; i++)
	{
		FrameBuffer[index][i] = 0;
	}
}

void MAX7219_MatrixSetRow(uint8_t index,  uint8_t rows[8])
{
	for(int i =0; i < 8; i++)
	{
		FrameBuffer[index][i] = rows[i];
	}
}

void MAX7219_MatrixSetRow64(uint8_t index,  uint64_t rows)
{
	for(int i =0; i < 8; i++)
	{
		uint8_t row = rows & 0xFF;
		FrameBuffer[index][7-i] = row;
		rows = rows >> 8;
	}
}

void MAX7219_MatrixSetPixel(uint8_t index, uint8_t digit, uint8_t row)
{
	FrameBuffer[index][digit] = row;
}

void MAX7219_MatrixRShift(uint8_t step)
{
	uint8_t msbD0, msbD1, msbD2, msbD3;

	for(int i=0; i < 8; i++)
	{
		msbD0 = ((FrameBuffer[0][i] & 0x80) >> 7);
		FrameBuffer[0][i] = FrameBuffer[0][i] << 1;

		msbD1 = ((FrameBuffer[1][i] & 0x80) >> 7);
		FrameBuffer[1][i] = FrameBuffer[1][i] << 1;
		FrameBuffer[1][i] |= msbD0;

		msbD2 = ((FrameBuffer[2][i] & 0x80) >> 7);
		FrameBuffer[2][i] = FrameBuffer[2][i] << 1;
		FrameBuffer[2][i] |= msbD1;

		msbD3 = ((FrameBuffer[3][i] & 0x80) >> 7);
		FrameBuffer[3][i] = FrameBuffer[3][i] << 1;
		FrameBuffer[3][i] |= msbD2;
		FrameBuffer[0][i] |= msbD3;
	}
}

void MAX7219_MatrixLShift(uint8_t step)
{
	uint8_t lsbD0, lsbD1, lsbD2, lsbD3;

	for(int i=0; i < 8; i++)
	{
		lsbD3 = ((FrameBuffer[3][i] & 0x01) << 7);
		FrameBuffer[3][i] = FrameBuffer[3][i] >> 1;

		lsbD2 = ((FrameBuffer[2][i] & 0x01) << 7);
		FrameBuffer[2][i] = FrameBuffer[2][i] >> 1;
		FrameBuffer[2][i] |= lsbD3;

		lsbD1 = ((FrameBuffer[1][i] & 0x01) << 7);
		FrameBuffer[1][i] = FrameBuffer[1][i] >> 1;
		FrameBuffer[1][i] |= lsbD2;

		lsbD0 = ((FrameBuffer[0][i] & 0x01) << 7);
		FrameBuffer[0][i] = FrameBuffer[0][i] >> 1;
		FrameBuffer[0][i] |= lsbD1;
		FrameBuffer[3][i] |= lsbD0;
	}
}

void MAX7219_MatrixUpdate()
{
	for(int index = 0; index < MAX7219_IC_NUM; index++)
	{
		for(int digit = 0; digit < 8; digit++)
		{
			uint8_t row = FrameBuffer[index][digit];
			MAX7219_Digit(index, digit, row);
		}
	}
}
