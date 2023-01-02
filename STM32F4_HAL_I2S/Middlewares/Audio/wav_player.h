/*
 * wav_player.h
 *
 *  Created on: 17 Apr 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef WAV_PLAYER_H_
#define WAV_PLAYER_H_

#include <stdbool.h>
#include <stdint.h>

#define WAV_MAX_NUM						20
#define AUDIO_BUFFER_SIZE  		4096

/* Audio buffer state */
typedef enum
{
	BUFFER_OFFSET_NONE = 0,
	BUFFER_OFFSET_HALF,
	BUFFER_OFFSET_FULL,
} BUFFER_StateTypeDef;

/* Player control state */
typedef enum
{
	PLAYER_CONTROL_IDLE = 0,
	PLAYER_CONTROL_HALF,
	PLAYER_CONTROL_FULL,
	PLAYER_CONTROL_EOF,
} PLAYER_ControlTypeDef;

/* Player state */
typedef enum
{
	PLAYBACK_STATE_IDLE = 0,
	PLAYBACK_STATE_FINDFILES,
	PLAYBACK_STATE_START,
	PLAYBACK_STATE_PLAY,
	PLAYBACK_STATE_STOP,
	PLAYBACK_STATE_PAUSE,
	PLAYBACK_STATE_RESUME,
	PLAYBACK_STATE_FORWARD,
	PLAYBACK_STATE_BACKWARD
} PLAYBACK_StateTypeDef;

/* WAV header structure */
typedef struct
{
	uint32_t ChunkID; 				/* 0  */
	uint32_t FileSize; 				/* 4  */
	uint32_t FileFormat; 			/* 8  */
	uint32_t SubChunk1ID; 		/* 12 */
	uint32_t SubChunk1Size; 	/* 16 */
	uint16_t AudioFormat; 		/* 20 */
	uint16_t NbrChannels; 		/* 22 */
	uint32_t SampleRate; 			/* 24 */
	uint32_t ByteRate; 				/* 28 */
	uint16_t BlockAlign; 			/* 32 */
	uint16_t BitPerSample; 		/* 34 */
	uint32_t SubChunk2ID; 		/* 36 */
	uint32_t SubChunk2Size; 	/* 40 */
} WAV_HeaderTypeDef;

/* external variables */
extern PLAYBACK_StateTypeDef playbackState;
extern uint8_t totalWavFileNum;
extern uint8_t currentFileIdx;
extern char wavFiles[WAV_MAX_NUM][255];
extern WAV_HeaderTypeDef wavHeaders[WAV_MAX_NUM];

/**
 * @brief find WAV files in the USB Drive
 */
void wavPlayer_findFiles(void);

/**
 * @brief Select WAV file to play
 * @retval returns true when file is found in USB Drive
 */
bool wavPlayer_fileSelect(const char *filePath);

/**
 * @brief WAV File Play
 */
void wavPlayer_play(void);

/**
 * @brief WAV stop
 */
void wavPlayer_stop(void);

/**
 * @brief Process WAV
 */
void wavPlayer_process(void);

/**
 * @brief isEndofFile reached
 */
bool wavPlayer_isFinished(void);

/**
 * @brief WAV pause/resume
 */
void wavPlayer_pause(void);
void wavPlayer_resume(void);

#endif /* WAV_PLAYER_H_ */
