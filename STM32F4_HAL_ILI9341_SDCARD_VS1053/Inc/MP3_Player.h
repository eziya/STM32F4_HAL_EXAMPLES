#ifndef MP3_PLAYER_H_
#define MP3_PLAYER_H_

#include <stdbool.h>
#include "stm32f4xx_hal.h"

#include "VS1053.h"

bool MP3_SetVolume(uint8_t volumeLeft, uint8_t volumeRight);
bool MP3_Init();
bool MP3_Play(const char *filename);

#endif /* MP3_PLAYER_H_ */
