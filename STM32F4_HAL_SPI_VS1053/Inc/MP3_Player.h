#ifndef MP3_PLAYER_H_
#define MP3_PLAYER_H_

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "VS1053.h"

/* Functions */
bool MP3_Init(void);
bool MP3_Play(const char *filename);
void MP3_Stop(void);
void MP3_Pause(void);
void MP3_Resume(void);
void MP3_Feeder(void);

/* Flags */
extern bool isPlaying;
extern bool isFileOpen;

#endif /* MP3_PLAYER_H_ */
