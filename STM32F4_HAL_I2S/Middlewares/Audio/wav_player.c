/*
 * wav_player.c
 *
 *  Created on: 17 Apr 2020
 *      Author: Mohamed Yaqoob
 */

#include "wav_player.h"
#include "audioI2S.h"
#include "fatfs.h"

/* Player external variables */
uint8_t totalWavFileNum = 0;
uint8_t currentFileIdx = 0;
char wavFiles[WAV_MAX_NUM][255];
WAV_HeaderTypeDef wavHeaders[WAV_MAX_NUM];
PLAYBACK_StateTypeDef playbackState = PLAYBACK_STATE_IDLE;

/* Player static variables */
static FIL wavFile;
static uint32_t fileLength;
static uint8_t audioBuffer[AUDIO_BUFFER_SIZE];
static uint32_t audioRemainSize = 0;
static uint32_t samplingFreq;
static UINT playerReadBytes = 0;
static bool isFinished = 0;
static PLAYER_ControlTypeDef playerControlSM = PLAYER_CONTROL_IDLE;

static void wavPlayer_reset(void)
{
	audioRemainSize = 0;
	playerReadBytes = 0;
}

void wavPlayer_findFiles(void)
{
	FRESULT fr;
	DIR dir;
	FILINFO fno;
	uint32_t readBytes;

	totalWavFileNum = 0;
	fr = f_findfirst(&dir, &fno, USBHPath, "????*.wav");

	while(fr == FR_OK && fno.fname[0])
	{
		if((totalWavFileNum >= WAV_MAX_NUM)) break;

		// copy file name & header info
		strncpy(wavFiles[totalWavFileNum], fno.fname, strlen(fno.fname));

		if(f_open(&wavFile, fno.fname, FA_READ) == FR_OK)
		{
			f_read(&wavFile, &wavHeaders[totalWavFileNum], sizeof(wavHeaders[0]), (UINT*)&readBytes);
			totalWavFileNum++;
		}

		f_close(&wavFile);
		fr = f_findnext(&dir, &fno);
	}

	f_closedir(&dir);
}

/**
 * @brief Select WAV file to play
 * @retval returns true when file is found in USB Drive
 */
bool wavPlayer_fileSelect(const char *filePath)
{
	WAV_HeaderTypeDef wavHeader;
	UINT readBytes = 0;

	if(f_open(&wavFile, filePath, FA_READ) != FR_OK) {return false;}

	// Retrieve file length & frequency info
	if(f_read(&wavFile, &wavHeader, sizeof(wavHeader), &readBytes) != FR_OK) {return false;}
	fileLength = wavHeader.FileSize;
	samplingFreq = wavHeader.SampleRate;
	return true;
}

/**
 * @brief WAV File Play
 */
void wavPlayer_play(void)
{
	// clear flag
	isFinished = false;

	// initialize I2S
	audioI2S_init(samplingFreq);

	// read data
	f_lseek(&wavFile, 0);
	f_read(&wavFile, &audioBuffer[0], AUDIO_BUFFER_SIZE, &playerReadBytes);
	audioRemainSize = fileLength - playerReadBytes;

	// start playing
	audioI2S_play((uint16_t*) &audioBuffer[0], AUDIO_BUFFER_SIZE);
}

/**
 * @brief Process WAV
 */
void wavPlayer_process(void)
{
	switch (playerControlSM)
	{
	case PLAYER_CONTROL_IDLE:
		break;

	case PLAYER_CONTROL_HALF:
		playerReadBytes = 0;
		playerControlSM = PLAYER_CONTROL_IDLE;
		f_read(&wavFile, &audioBuffer[0], AUDIO_BUFFER_SIZE / 2, &playerReadBytes);
		if(audioRemainSize > (AUDIO_BUFFER_SIZE / 2))
		{
			audioRemainSize -= playerReadBytes;
		}
		else
		{
			audioRemainSize = 0;
			playerControlSM = PLAYER_CONTROL_EOF;
		}
		break;

	case PLAYER_CONTROL_FULL:
		playerReadBytes = 0;
		playerControlSM = PLAYER_CONTROL_IDLE;
		f_read(&wavFile, &audioBuffer[AUDIO_BUFFER_SIZE / 2], AUDIO_BUFFER_SIZE / 2, &playerReadBytes);
		if(audioRemainSize > (AUDIO_BUFFER_SIZE / 2))
		{
			audioRemainSize -= playerReadBytes;
		}
		else
		{
			audioRemainSize = 0;
			playerControlSM = PLAYER_CONTROL_EOF;
		}
		break;

	case PLAYER_CONTROL_EOF:
		f_close(&wavFile);
		wavPlayer_reset();
		isFinished = true;
		playerControlSM = PLAYER_CONTROL_IDLE;
		break;
	}
}

/**
 * @brief WAV stop
 */
void wavPlayer_stop(void)
{
	audioI2S_stop();
	wavPlayer_reset();
	f_close(&wavFile);
	isFinished = true;
}

/**
 * @brief WAV pause/resume
 */
void wavPlayer_pause(void)
{
	audioI2S_pause();
}
void wavPlayer_resume(void)
{
	audioI2S_resume();
}

/**
 * @brief isEndofFile reached
 */
bool wavPlayer_isFinished(void)
{
	return isFinished;
}

/**
 * @brief Half/Full transfer Audio callback for buffer management
 */
void audioI2S_halfTransfer_Callback(void)
{
	playerControlSM = PLAYER_CONTROL_HALF;
}
void audioI2S_fullTransfer_Callback(void)
{
	playerControlSM = PLAYER_CONTROL_FULL;
}
