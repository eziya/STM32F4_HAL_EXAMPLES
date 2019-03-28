/*
 * max7219.h
 *
 *  Created on: 2019. 3. 22.
 *      Author: KIKI
 */

#ifndef MAX7219_H_
#define MAX7219_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define MAX7219_REG_NOOP		0x00
#define MAX7219_REG_D0			0x01
#define MAX7219_REG_D1			0x02
#define MAX7219_REG_D2			0x03
#define MAX7219_REG_D3			0x04
#define MAX7219_REG_D4			0x05
#define MAX7219_REG_D5			0x06
#define MAX7219_REG_D6			0x07
#define MAX7219_REG_D7			0x08
#define MAX7219_REG_DECODE		0x09
#define MAX7219_REG_INTENSITY	0x0A
#define MAX7219_REG_SCANLIMIT	0x0B
#define MAX7219_REG_SHUTDOWN	0x0C
#define MAX7219_REG_TEST		0x0F

#define MAX7219_IC_NUM		4

void MAX7219_Init(SPI_HandleTypeDef* spi, GPIO_TypeDef* ss_port, uint16_t ss_pin);
bool MAX7219_Write(uint8_t index, uint8_t reg, uint8_t data);
bool MAX7219_Digit(uint8_t index, uint8_t digit, int8_t value);
bool MAX7219_Decode(uint8_t index, uint8_t value);
bool MAX7219_Intensity(uint8_t index, uint8_t value);
bool MAX7219_ScanLimit(uint8_t index, uint8_t value);
bool MAX7219_ShutDown(uint8_t index, uint8_t value);
bool MAX7219_Test(uint8_t index, uint8_t value);

#endif /* MAX7219_H_ */
