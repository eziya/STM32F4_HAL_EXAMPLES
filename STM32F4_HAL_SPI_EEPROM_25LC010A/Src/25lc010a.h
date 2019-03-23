/*
 * 25lc010a.h
 *
 *  Created on: 2019. 3. 22.
 *      Author: KIKI
 */

#ifndef _25LC010A_H_
#define _25LC010A_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define EEPROM_CMD_WRITE	0x02
#define EEPROM_CMD_READ		0x03
#define EEPROM_CMD_RDSR		0x05
#define EEPROM_CMD_WREN		0x06

#define EEPROM_PAGE_SIZE	16
#define EEPROM_TOTAL_SIZE	128

void EEPROM_Init(SPI_HandleTypeDef* spi, GPIO_TypeDef* ss_port, uint16_t ss_pin);
bool EEPROM_WriteEnable(void);
bool EEPROM_ReadByte(uint8_t address, uint8_t *ret);
bool EEPROM_ReadBuffer(uint8_t address, uint8_t *buffer, uint8_t length);
bool EEPROM_WriteByte(uint8_t address, uint8_t data);
bool EEPROM_WriteBuffer(uint8_t address, uint8_t *buffer, uint8_t length);
bool EEPROM_EraseAll(void);

#endif /* _25LC010A_H_ */
