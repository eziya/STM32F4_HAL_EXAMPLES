/*
 * max7219_matrix.c
 *
 *  Created on: 2019. 3. 22.
 *      Author: KIKI
 */


#include "max7219_matrix.h"
#include "max7219.h"
#include <string.h>

const uint64_t symbol[] = {
		0x1800183860663c00,
		0x10387cfefeee4400,
		0x060e0c0808281800,
		0x10307efe7e301000,
		0x1018fcfefc181000
};

const uint64_t numbers[] = {
		/* 8     =>      1   row */
		0x3c66666e76663c00, /* 0 */
		0x7e1818181c181800,	/* 1 */
		0x7e060c3060663c00,	/* 2 */
		0x3c66603860663c00,	/* 3 */
		0x30307e3234383000, /* 4 */
		0x3c6660603e067e00, /* 5 */
		0x3c66663e06663c00, /* 6 */
		0x1818183030667e00, /* 7 */
		0x3c66663c66663c00, /* 8 */
		0x3c66607c66663c00, /* 9 */
};

const uint64_t Atoz[] = {
		0x6666667e66663c00, /* A */
		0x3e66663e66663e00,	/* B */
		0x3c66060606663c00,
		0x3e66666666663e00,
		0x7e06063e06067e00,
		0x0606063e06067e00,
		0x3c66760606663c00,
		0x6666667e66666600,
		0x3c18181818183c00,
		0x1c36363030307800,
		0x66361e0e1e366600,
		0x7e06060606060600,
		0xc6c6c6d6feeec600,
		0xc6c6e6f6decec600,
		0x3c66666666663c00,
		0x06063e6666663e00,
		0x603c766666663c00,
		0x66361e3e66663e00,
		0x3c66603c06663c00,
		0x18181818185a7e00,
		0x7c66666666666600,
		0x183c666666666600,
		0xc6eefed6c6c6c600,
		0xc6c66c386cc6c600,
		0x1818183c66666600,
		0x7e060c1830607e00,
		0x0000000000000000,
		0x7c667c603c000000,
		0x3e66663e06060600,
		0x3c6606663c000000,
		0x7c66667c60606000,
		0x3c067e663c000000,
		0x0c0c3e0c0c6c3800,
		0x3c607c66667c0000,
		0x6666663e06060600,
		0x3c18181800180000,
		0x1c36363030003000,
		0x66361e3666060600,
		0x1818181818181800,
		0xd6d6feeec6000000,
		0x6666667e3e000000,
		0x3c6666663c000000,
		0x06063e66663e0000,
		0xf0b03c36363c0000,
		0x060666663e000000,
		0x3e403c027c000000,
		0x1818187e18180000,
		0x7c66666666000000,
		0x183c666600000000,
		0x7cd6d6d6c6000000,
		0x663c183c66000000,
		0x3c607c6666000000, /* y */
		0x3c0c18303c000000  /* z */
};

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

bool MAX7219_MatrixUpdate()
{
	for(int index = 0; index < MAX7219_IC_NUM; index++)
	{
		for(int digit = 0; digit < 8; digit++)
		{
			uint8_t row = FrameBuffer[index][digit];
			if(!MAX7219_Digit(index, digit, row)) return false;
		}
	}

	return true;
}
