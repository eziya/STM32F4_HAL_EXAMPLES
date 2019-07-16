#include "stdint.h"

#ifndef _VS1053_H_
#define _VS1053_H_

#include <stdbool.h>
#include "stm32f4xx_hal.h"

/* Pin configuration */
extern SPI_HandleTypeDef 			hspi2;
#define HSPI_VS1053					&hspi2
#define VS1053_DREQ_PORT			GPIOD
#define VS1053_DREQ_PIN				GPIO_PIN_11
/* Adafruit Music Maker has no XRST pin
#define	VS1053_XRST_PORT			GPIOD
#define	VS1053_XRST_PIN				GPIO_PIN_1
 */
#define VS1053_XCS_PORT				GPIOD
#define VS1053_XCS_PIN				GPIO_PIN_9
#define VS1053_XDCS_PORT			GPIOD
#define VS1053_XDCS_PIN				GPIO_PIN_10


/* Functions */
bool VS1053_Init();
void VS1053_Reset();
bool VS1053_SoftReset();
bool VS1053_SetVolume(uint8_t volumeLeft, uint8_t volumeRight);
bool VS1053_SetMode(uint16_t mode);
bool VS1053_GetMode(uint16_t *mode);
bool VS1053_AutoResync();
bool VS1053_SetDecodeTime(uint16_t time);
bool VS1053_SendEndFill(uint16_t num);
bool VS1053_IsBusy();
bool VS1053_SciWrite(uint8_t address, uint16_t input);
bool VS1053_SciRead(uint8_t address, uint16_t *res);
bool VS1053_SdiWrite(uint8_t input);
bool VS1053_SdiWrite32(uint8_t *input32);

extern uint8_t endFillByte;

#endif
