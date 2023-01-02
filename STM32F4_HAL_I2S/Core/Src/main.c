/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "i2s.h"
#include "usart.h"
#include "usb_host.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "audioI2S.h"
#include "cs43l22.h"
#include "wav_player.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define LED1_ON 			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET)
#define LED1_OFF 			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET)
#define LED1_TOGGLE		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12)
#define LED2_ON 			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET)
#define LED2_OFF 			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET)
#define LED2_TOGGLE		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13)
#define LED3_ON 			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET)
#define LED3_OFF 			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET)
#define LED3_TOGGLE		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14)
#define LED4_ON 			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET)
#define LED4_OFF 			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET)
#define LED4_TOGGLE		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15)

/* UART */
uint8_t rxBuf;

int _write(int file, char *p, int len)
{
	HAL_UART_Transmit(&huart2, (const uint8_t*)p, len, 10);
	return len;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	playbackState = rxBuf - '0';
	HAL_UART_Receive_IT(huart, &rxBuf, 1);
}
/* UART */

/* USB Mass Storage */
extern ApplicationTypeDef Appli_state;

void MountUSB(void)
{
	FRESULT res = f_mount(&USBHFatFS, USBHPath, 0);
	if(res != FR_OK) {Error_Handler();}
	LED1_ON;
}

void UnMountUSB(void)
{
	FRESULT res = f_mount(NULL, "", 0);
	if(res != FR_OK) {Error_Handler();}
	LED1_OFF;
}
/* USB Mass Storage */

/* Wave Player */
void HandlePlayBackFindFiles(void)
{
	wavPlayer_stop();
	wavPlayer_findFiles();
	printf("WAV file list...\r\n");
	for(int i = 0; i < totalWavFileNum; i++)
	{
		printf("[%02d] File Name: %s, Sampling Rate: %ld bps\r\n", (i + 1), wavFiles[i], wavHeaders[i].SampleRate);
	}
	playbackState = PLAYBACK_STATE_IDLE;
	currentFileIdx = 0;
}

void HandlePlayBackStart(void)
{
	printf("Playing file: %s / %ld bps\r\n", wavFiles[currentFileIdx], wavHeaders[currentFileIdx].SampleRate);
	wavPlayer_fileSelect(wavFiles[currentFileIdx]);
	wavPlayer_play();
	playbackState = PLAYBACK_STATE_PLAY;
}

void HandlePlayBackPlay(void)
{
	wavPlayer_process();
	LED2_ON;
	if(wavPlayer_isFinished())
	{
		playbackState = PLAYBACK_STATE_STOP;
	}
}

void HandlePlayBackStop(void)
{
	printf("Stop playing.\r\n");
	wavPlayer_stop();
	LED2_OFF;
	playbackState = PLAYBACK_STATE_IDLE;
}

void HandlePlayBackPause(void)
{
	printf("Paused.\r\n");
	wavPlayer_pause();
	LED2_OFF;
	playbackState = PLAYBACK_STATE_IDLE;
}

void HandlePlayBackResume(void)
{
	printf("Resume.\r\n");
	wavPlayer_resume();
	playbackState = PLAYBACK_STATE_PLAY;
}

void HandlePlayBackForward(void)
{
	printf("Going forward.\r\n");
	wavPlayer_stop();
	currentFileIdx++;
	if(currentFileIdx >= totalWavFileNum) { currentFileIdx = 0;}
	playbackState = PLAYBACK_STATE_START;
}

void HandlePlayBackBackward(void)
{
	printf("Going backward.\r\n");
	wavPlayer_stop();
	currentFileIdx--;
	if(currentFileIdx >= totalWavFileNum) { currentFileIdx = (totalWavFileNum - 1);}
	playbackState = PLAYBACK_STATE_START;
}

void HandlePlayBackDisplayMenu(void)
{
	printf("Command List...\r\n");
	printf("1: Find files.\r\n");
	printf("2: Start playing.\r\n");
	printf("3: Playing.\r\n");
	printf("4: Stop.\r\n");
	printf("5: Pause.\r\n");
	printf("6: Resume.\r\n");
	printf("7: Forward.\r\n");
	printf("8: Backward.\r\n");

	if(wavPlayer_isFinished())
	{
		playbackState = PLAYBACK_STATE_IDLE;
	}
	else
	{
		playbackState = PLAYBACK_STATE_PLAY;
	}
}

void HandlePlayBack(void)
{
	if((playbackState != PLAYBACK_STATE_IDLE) && (playbackState != PLAYBACK_STATE_FINDFILES))
	{
		if(wavHeaders[currentFileIdx].SampleRate == 0 || wavHeaders[currentFileIdx].FileSize == 0)
		{
			printf("Find WAV files first...\r\n");
			HandlePlayBackDisplayMenu();
		}
	}

	switch(playbackState)
	{
	case PLAYBACK_STATE_IDLE:
		break;
	case PLAYBACK_STATE_FINDFILES:
		HandlePlayBackFindFiles();
		break;
	case PLAYBACK_STATE_START:
		HandlePlayBackStart();
		break;
	case PLAYBACK_STATE_PLAY:
		HandlePlayBackPlay();
		break;
	case PLAYBACK_STATE_STOP:
		HandlePlayBackStop();
		break;
	case PLAYBACK_STATE_PAUSE:
		HandlePlayBackPause();
		break;
	case PLAYBACK_STATE_RESUME:
		HandlePlayBackResume();
		break;
	case PLAYBACK_STATE_FORWARD:
		HandlePlayBackForward();
		break;
	case PLAYBACK_STATE_BACKWARD:
		HandlePlayBackBackward();
		break;
	default:
		HandlePlayBackDisplayMenu();
		break;
	}
}
/* Wave Player */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_I2C1_Init();
	MX_I2S3_Init();
	MX_FATFS_Init();
	MX_USB_HOST_Init();
	MX_USART2_UART_Init();
	/* USER CODE BEGIN 2 */

	// 2022.12.29 MX_DriverVbusFS code generation is not correct. Control PC0 manually.
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);

	// Initialize CS43L22
	CS43_Init(hi2c1, CS43_MODE_I2S);
	CS43_SetVolume(160);
	CS43_Enable_RightLeft(CS43_RIGHT_LEFT);
	audioI2S_setHandle(&hi2s3);

	// Enable UART2
	HAL_UART_Receive_IT(&huart2, &rxBuf, 1);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while(1)
	{
		/* USER CODE END WHILE */
		MX_USB_HOST_Process();

		/* USER CODE BEGIN 3 */
		if(Appli_state == APPLICATION_START)
		{
			// mount file system
			MountUSB();
		}
		else if(Appli_state == APPLICATION_READY)
		{
			// Handle player
			HandlePlayBack();
		}
		else if(Appli_state == APPLICATION_DISCONNECT)
		{
			wavPlayer_stop();
			UnMountUSB();
		}
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while(1)
	{
		LED3_ON;
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
