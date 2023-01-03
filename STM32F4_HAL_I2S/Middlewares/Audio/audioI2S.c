/*
 * audioI2S.c
 *
 *  Created on: 17 Apr 2020
 *      Author: Mohamed Yaqoob
 */

#include "cs43l22.h"
#include "audioI2S.h"
#include "stm32f4xx_hal.h"

extern void Error_Handler(void);

// Refer to STM32F407 reference manual Table.127 when master clock is enabled.
const uint32_t I2SFreq[7] =
{ 8000, 16000, 32000, 48000, 96000, 22050, 44100};
const uint32_t I2SPLLN[7] =
{ 256, 213, 213, 258, 344, 429, 271};
const uint32_t I2SPLLR[7] =
{ 5, 2, 2, 3, 2, 4, 2 };

static I2S_HandleTypeDef *hAudioI2S;

/**
 * @brief I2S Clock Config
 */
static void audioI2S_pllClockConfig(uint32_t audioFreq)
{
	RCC_PeriphCLKInitTypeDef rccclkinit;
	uint8_t index = 0, freqindex = 0xFF;

	for(index = 0; index < 7; index++)
	{
		if(I2SFreq[index] == audioFreq)
		{
			freqindex = index;
		}
	}
	/* Enable PLLI2S clock */
	HAL_RCCEx_GetPeriphCLKConfig(&rccclkinit);

	if((freqindex & 0x6) != 0)
	{
		rccclkinit.PeriphClockSelection = RCC_PERIPHCLK_I2S;
		rccclkinit.PLLI2S.PLLI2SN = I2SPLLN[freqindex];
		rccclkinit.PLLI2S.PLLI2SR = I2SPLLR[freqindex];
		if(HAL_RCCEx_PeriphCLKConfig(&rccclkinit) != HAL_OK) {Error_Handler();}
	}
	else
	{
		/* default 44.1kHz */
		rccclkinit.PeriphClockSelection = RCC_PERIPHCLK_I2S;
		rccclkinit.PLLI2S.PLLI2SN = 271;
		rccclkinit.PLLI2S.PLLI2SR = 2;
		if(HAL_RCCEx_PeriphCLKConfig(&rccclkinit) != HAL_OK) {Error_Handler();}
	}
}

/**
 * @brief update I2S peripheral with selected Sampling Frequency
 */
static void I2S3_freqUpdate(uint32_t AudioFreq)
{
	/* Initialize the hAudioOutI2s Instance parameter */
	hAudioI2S->Instance = SPI3;

	/* Disable I2S block */
	__HAL_I2S_DISABLE(hAudioI2S);

	/* I2S3 peripheral configuration */
	hAudioI2S->Init.AudioFreq = AudioFreq;
	hAudioI2S->Init.ClockSource = I2S_CLOCK_PLL;
	hAudioI2S->Init.CPOL = I2S_CPOL_LOW;
	hAudioI2S->Init.DataFormat = I2S_DATAFORMAT_16B;
	hAudioI2S->Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
	hAudioI2S->Init.Mode = I2S_MODE_MASTER_TX;
	hAudioI2S->Init.Standard = I2S_STANDARD_PHILIPS;

	/* Reinitialize based on new audio frequency */
	if(HAL_I2S_Init(hAudioI2S) != HAL_OK) {Error_Handler();}
}

/**
 * @brief set I2S HAL handle
 */
void audioI2S_setHandle(I2S_HandleTypeDef *pI2Shandle)
{
	hAudioI2S = pI2Shandle;
}

/* I2S Audio library function definitions */
/**
 * @brief Initialises I2S Audio settings
 * @param audioFreq - WAV file Audio sampling rate (44.1KHz, 48KHz, ...)
 */
void audioI2S_init(uint32_t audioFreq)
{
	//Update PLL & I2S frequency
	audioI2S_pllClockConfig(audioFreq);
	I2S3_freqUpdate(audioFreq);
}

/**
 * @brief Starts Playing Audio from buffer
 */
bool audioI2S_play(uint16_t *pDataBuf, uint32_t len)
{
	CS43_Start();
	HAL_I2S_Transmit_DMA(hAudioI2S, pDataBuf, DMA_MAX(len/AUDIODATA_SIZE));
	return true;
}

/**
 * @brief Change I2S DMA Buffer pointer
 */
bool audioI2S_changeBuffer(uint16_t *pDataBuf, uint32_t len)
{
	HAL_I2S_Transmit_DMA(hAudioI2S, pDataBuf, DMA_MAX(len));
	return true;
}

/**
 * @brief Pause audio out
 */
void audioI2S_pause(void)
{
	CS43_Stop();
	HAL_I2S_DMAPause(hAudioI2S);
}

/**
 * @brief Resume audio out
 */
void audioI2S_resume(void)
{
	CS43_Start();
	HAL_I2S_DMAResume(hAudioI2S);
}

/**
 * @brief Set Volume
 */
void audioI2S_setVolume(uint8_t volume)
{
	CS43_SetVolume(volume);
}

/**
 * @brief Stop audio
 */
void audioI2S_stop(void)
{
	CS43_Stop();
	HAL_I2S_DMAStop(hAudioI2S);
}

/**
 * @brief Half/Full transfer Audio callback for buffer management
 */
__weak void audioI2S_halfTransfer_Callback(void)
{

}
__weak void audioI2S_fullTransfer_Callback(void)
{

}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	if(hi2s->Instance == SPI3)
	{
		audioI2S_fullTransfer_Callback();
	}
}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	if(hi2s->Instance == SPI3)
	{
		audioI2S_halfTransfer_Callback();
	}
}
