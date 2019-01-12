/*
 * NOKIA5110.h
 *
 * Created: 2018-03-11 오후 8:39:05
 *  Author: kiki
 */ 


#ifndef __NOKIA5110_H_
#define __NOKIA5110_H_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define N5110_CMD		                0
#define N5110_DATA		                1
#define N5110_MAX_WIDTH		                84
#define N5110_MAX_HEIGHT	                48
#define N5110_PIXEL			        504

#define PCD8544_BIT_POWERDOWN			0x04
#define PCD8544_BIT_ENTRYMODE			0x02
#define PCD8544_BIT_EXTENDED			0x01
#define PCD8544_BIT_DISPLAYBLANK		0x00
#define PCD8544_BIT_DISPLAYNORMAL		0x04
#define PCD8544_BIT_DISPLAYALLON		0x01
#define PCD8544_BIT_DISPLAYINVERTED		0x05

#define PCD8544_CMD_FUNCTIONSET			0x20
#define PCD8544_CMD_DISPLAYCONTROL		0x08
#define PCD8544_CMD_SETYADDR			0x40
#define PCD8544_CMD_SETXADDR			0x80
#define PCD8544_CMD_SETTEMP			0x04
#define PCD8544_CMD_SETBIAS			0x10
#define PCD8544_CMD_SETVOP			0x80

#define PCD8544_DEFAULT_BIAS			0x03
#define PCD8544_DEFAULT_TEMP			0x03
#define PCD8544_DEFAULT_CONTRAST		0x40

extern uint8_t FrameBuffer[N5110_PIXEL];

bool N5110_Init(void);
void N5110_Reset(void);
bool N5110_SetContrast(uint8_t contrast);
bool N5110_InvertDisplay(bool invert);
bool N5110_Update(void);
void N5110_Clear(void);
void N5110_Fill(void);
void N5110_SetPixel(uint8_t x, uint8_t y, bool set);
void N5110_SetFont(const uint8_t * font);
void N5110_PrintChar(uint8_t x, uint8_t y, char ch, bool set);
void N5110_PrintStr(uint8_t x, uint8_t y, char *str, bool set);
void N5110_DrawHLine(uint8_t x, uint8_t y, uint8_t length, bool set);
void N5110_DrawVLine(uint8_t x, uint8_t y, uint8_t length, bool set);
void N5110_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool set);
void N5110_DrawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool set, bool fill);
void N5110_DrawRoundRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool set, bool fill);
void N5110_DrawCircle(uint8_t x, uint8_t y, uint8_t radius, bool set, bool fill);
void N5110_DrawBitmap(uint8_t x, uint8_t y, uint8_t * bitmap, uint8_t width, uint8_t height);

#endif /* NOKIA5110_H_ */
