/*
 * NOKIA5110.c
 *
 * Created: 2018-03-11 오후 8:39:25
 *  Author: kiki
 */

#include "main.h"
#include "stm32f4xx_hal.h"

#include "NOKIA5110.h"
#include "FONT_IMAGE.h"

#define SPI_Select()    HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_RESET)
#define SPI_Deselect()  HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_SET)

extern SPI_HandleTypeDef hspi2;

uint8_t FrameBuffer[N5110_PIXEL];
static const uint8_t *CurrentFont = SmallFont;

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* Deselect when Tx Complete */
  if(hspi->Instance == SPI2)
  {
    SPI_Deselect();
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
  }
}


static bool N5110_Write(uint8_t data, uint8_t mode)
{
  SPI_Select();

  if(mode == N5110_CMD)
  {
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
  }
  else if(mode == N5110_DATA)
  {
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
  }
  else
  {
    SPI_Deselect();
    return false;
  }

  HAL_SPI_Transmit_DMA(&hspi2, &data, 1);
  //HAL_SPI_Transmit(&hspi2, &data, 1, HAL_MAX_DELAY);
  //SPI_Deselect();

  return true;
}

static bool N5110_BufferWrite(uint8_t *data, uint16_t len)
{
  SPI_Select();
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
  HAL_SPI_Transmit_DMA(&hspi2, data, len);
  //HAL_SPI_Transmit(&hspi2, data, len, HAL_MAX_DELAY);
  //SPI_Deselect();

  return true;
}

bool N5110_Init()
{
  N5110_Reset();
  if(!N5110_Write(PCD8544_CMD_FUNCTIONSET | PCD8544_BIT_EXTENDED, N5110_CMD)) return false;
  if(!N5110_Write(PCD8544_CMD_SETVOP | PCD8544_DEFAULT_CONTRAST, N5110_CMD)) return false;
  if(!N5110_Write(PCD8544_CMD_SETTEMP | PCD8544_DEFAULT_TEMP, N5110_CMD)) return false;
  if(!N5110_Write(PCD8544_CMD_SETBIAS | PCD8544_DEFAULT_BIAS, N5110_CMD)) return false;
  if(!N5110_Write(PCD8544_CMD_FUNCTIONSET, N5110_CMD)) return false;
  if(!N5110_Write(PCD8544_CMD_DISPLAYCONTROL | PCD8544_BIT_DISPLAYNORMAL, N5110_CMD)) return false;

  return true;
}

void N5110_Reset()
{
  HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
}

bool N5110_SetContrast(uint8_t contrast)
{
  /* Max Contrast */
  if(contrast > 0x7F) contrast = 0x7F;

  if(!N5110_Write(PCD8544_CMD_FUNCTIONSET | PCD8544_BIT_EXTENDED, N5110_CMD)) return false;
  if(!N5110_Write(PCD8544_CMD_SETVOP | contrast, N5110_CMD)) return false;
  if(!N5110_Write(PCD8544_CMD_FUNCTIONSET, N5110_CMD)) return false;

  return true;
}

bool N5110_InvertDisplay(bool invert)
{
  if(invert)
  {
    if(!N5110_Write(PCD8544_CMD_DISPLAYCONTROL | PCD8544_BIT_DISPLAYINVERTED, N5110_CMD)) return false;
  }
  else
  {
    if(!N5110_Write(PCD8544_CMD_DISPLAYCONTROL | PCD8544_BIT_DISPLAYNORMAL, N5110_CMD)) return false;
  }

  return true;
}

bool N5110_Update()
{
  /* Move to (0,0) position */
  if(!N5110_Write(PCD8544_CMD_SETXADDR, N5110_CMD)) return false;
  if(!N5110_Write(PCD8544_CMD_SETYADDR, N5110_CMD)) return false;

  if(!N5110_BufferWrite(FrameBuffer, N5110_PIXEL)) return false;
  HAL_Delay(10);

  return true;
}

void N5110_Clear()
{
  for(int i = 0; i < N5110_PIXEL; i++)
  {
    FrameBuffer[i] = 0x00;
  }
}

void N5110_Fill()
{
  for(int i = 0; i < N5110_PIXEL; i++)
  {
    FrameBuffer[i] = 0xFF;
  }
}

void N5110_SetPixel(uint8_t x, uint8_t y, bool set)
{
  uint16_t by;
  uint8_t  bi;

  if(x < 0 || x >= N5110_MAX_WIDTH || y < 0 || y >= N5110_MAX_HEIGHT) return;

  by = (y / 8) * N5110_MAX_WIDTH + x;
  bi = y % 8;

  if(set)
  {
    FrameBuffer[by] |= (1 << bi);
  }
  else
  {
    FrameBuffer[by] &= ~(1 << bi);
  }
}

void N5110_SetFont(const uint8_t * font)
{
  CurrentFont = font;
}

void N5110_PrintChar(uint8_t x, uint8_t y, char ch, bool set)
{
  uint8_t f_width  = CurrentFont[0];
  uint8_t f_height = CurrentFont[1];
  uint8_t f_offset = CurrentFont[2];

  if((f_height % 8) == 0)
  {
    uint16_t f_index = (ch - f_offset) * (f_width * f_height / 8) + FONT_HEADER_SIZE;
    for(int row = 0; row < f_height / 8; row++)
    {
      for(int cx = 0; cx < f_width; cx++)
      {
        for(int bi = 0; bi < 8; bi++)
        {
          uint8_t cbyte = CurrentFont[f_index + cx + (row * f_width)];
          if((cbyte & (1 << bi)) != 0)
          {
            N5110_SetPixel(x + cx, y + row * 8 + bi, set);
          }
          else
          {
            N5110_SetPixel(x + cx, y + row * 8 + bi, !set);
          }
        }
      }
    }
  }
  else
  {
    uint16_t f_index = (ch - f_offset) * (f_width * f_height / 8) + FONT_HEADER_SIZE;
    uint8_t cbyte = CurrentFont[f_index];
    int8_t bi = 7;

    for(int cx = 0; cx < f_width; cx++)
    {
      for(int cy = 0; cy < f_height; cy++)
      {
        if((cbyte & (1 << bi)) != 0)
        {
          N5110_SetPixel(x+cx, y+cy, set);
        }
        else
        {
          N5110_SetPixel(x+cx, y+cy, !set);
        }

        bi--;

        if(bi < 0)
        {
          bi = 7;
          f_index++;
          cbyte = CurrentFont[f_index];
        }
      }
    }
  }
}

void N5110_PrintStr(uint8_t x, uint8_t y, char *str, bool set)
{
  uint8_t f_width  = CurrentFont[0];
  uint8_t length = strlen(str);

  for(int i = 0; i < length; i++)
  {
    N5110_PrintChar(x + (i * f_width), y, *str++, set);
  }
}

void N5110_DrawHLine(uint8_t x, uint8_t y, uint8_t length, bool set)
{
  for(int cx = 0; cx < length; cx++)
  {
    N5110_SetPixel(x+cx, y, set);
  }
}

void N5110_DrawVLine(uint8_t x, uint8_t y, uint8_t length, bool set)
{
  for(int cy = 0; cy < length; cy++)
  {
    N5110_SetPixel(x, y+cy, set);
  }
}

void N5110_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool set)
{
  uint8_t tmp;
  float delta, tx, ty;

  if(x1 > x2)
  {
    tmp = x1;
    x1 = x2;
    x2 = tmp;

    tmp = y1;
    y1 = y2;
    y2 = tmp;
  }

  if(y1 > y2)
  {
    tmp = x1;
    x1 = x2;
    x2 = tmp;

    tmp = y1;
    y1 = y2;
    y2 = tmp;
  }

  if(y1 == y2)
  {
    if(x1 > x2)
    {
      tmp = x1;
      x1 = x2;
      x2 = tmp;
    }

    N5110_DrawHLine(x1, y1, x2-x1, set);
  }
  else if(x1 == x2)
  {
    if(y1 > y2)
    {
      tmp = y1;
      y1 = y2;
      y2 = tmp;
    }

    N5110_DrawVLine(x1, y1, y2-y1, set);
  }
  else if (abs(x2-x1)>abs(y2-y1))
  {
    delta = (float)((float)(y2-y1)/(float)(x2-x1));
    ty = (float)y1;
    if (x1 > x2)
    {
      for (int8_t i = x1; i >= x2; i--)
      {
        N5110_SetPixel(i, (uint8_t)(ty+0.5), set);
        ty = ty-delta;
      }
    }
    else
    {
      for (uint8_t i = x1; i <= x2; i++)
      {
        N5110_SetPixel(i, (uint8_t)(ty+0.5), set);
        ty = ty+delta;
      }
    }
  }
  else
  {
    delta = (float)((float)(x2-x1)/(float)(y2-y1));
    tx = (float)x1;
    if(y1 > y2)
    {
      for(int i = y2+1; i > y1; i--)
      {
        N5110_SetPixel((uint8_t)(tx+0.5), i, set);
        tx = tx+delta;
      }
    }
    else
    {
      for (int i = y1; i < y2+1; i++)
      {
        N5110_SetPixel((uint8_t)(tx+0.5), i, set);
        tx=tx+delta;
      }
    }
  }
}

void N5110_DrawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool set, bool fill)
{
  uint8_t tmp;

  /* Swap position to make x2 and y2 are bigger than x1 and y1 */
  if(x2 < x1)
  {
    tmp = x1;
    x1 = x2;
    x2 = tmp;
  }

  if(y2 < y1)
  {
    tmp = y1;
    y1 = y2;
    y2 = tmp;
  }

  if(fill)
  {
    for(int i = x1; i <= x2; i++)
    {
      for(int j = y1; j <= y2; j++)
      {
        N5110_SetPixel(i, j, set);
      }
    }
  }
  else
  {
    N5110_DrawHLine(x1, y1, x2-x1, set);
    N5110_DrawHLine(x1, y2, x2-x1, set);
    N5110_DrawVLine(x1, y1, y2-y1, set);
    N5110_DrawVLine(x2, y1, y2-y1, set);
  }
}

void N5110_DrawRoundRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool set, bool fill)
{
  uint8_t tmp;

  /* Swap position to make x2 and y2 are bigger than x1 and y1 */
  if(x2 < x1)
  {
    tmp = x1;
    x1 = x2;
    x2 = tmp;
  }

  if(y2 < y1)
  {
    tmp = y1;
    y1 = y2;
    y2 = tmp;
  }

  /* Too small to make rounded rectangle */
  if((x2-x1 <= 4) || (y2-y1) <= 4) return;

  if(fill)
  {
    for(int i = x1; i <= x2; i++)
    {
      for(int j = y1; j <= y2; j++)
      {
        /* Rounds four corners of rectangle */
        if( (i== x1 && j == y1) || (i == x1 && j == y1+1) || (i == x1+1 && j == y1 ) ||
            (i== x1 && j == y2) || (i == x1 && j == y2-1) || (i == x1+1 && j == y2 ) ||
            (i== x2 && j == y1) || (i == x2 && j == y1+1) || (i == x2-1 && j == y1 ) ||
            (i== x2 && j == y2) || (i == x2 && j == y2-1) || (i == x2-1 && j == y2 ) )
        {
          N5110_SetPixel(i, j, !set);
        }
        else
        {
          N5110_SetPixel(i, j, set);
        }
      }
    }
  }
  else
  {
    N5110_SetPixel(x1+1, y1+1, set);
    N5110_SetPixel(x2-1, y1+1, set);
    N5110_SetPixel(x1+1, y2-1, set);
    N5110_SetPixel(x2-1, y2-1, set);

    N5110_DrawHLine(x1+2, y1, x2-x1-3, set);
    N5110_DrawHLine(x1+2, y2, x2-x1-3, set);
    N5110_DrawVLine(x1, y1+2, y2-y1-3, set);
    N5110_DrawVLine(x2, y1+2, y2-y1-3, set);
  }
}

void N5110_DrawCircle(uint8_t x, uint8_t y, uint8_t radius, bool set, bool fill)
{
  int8_t a = 0;
  int8_t b = radius;
  int8_t p = 1 - radius;

  do
  {
    if(fill)
    {
      N5110_DrawLine(x-a, y+b, x+a, y+b, set);
      N5110_DrawLine(x-a, y-b, x+a, y-b, set);
      N5110_DrawLine(x-b, y+a, x+b, y+a, set);
      N5110_DrawLine(x-b, y-a, x+b, y-a, set);
    }
    else
    {
      N5110_SetPixel(x+a, y+b, set);
      N5110_SetPixel(x-a, y+b, set);
      N5110_SetPixel(x+a, y-b, set);
      N5110_SetPixel(x-a, y-b, set);
      N5110_SetPixel(x+b, y+a, set);
      N5110_SetPixel(x-b, y+a, set);
      N5110_SetPixel(x+b, y-a, set);
      N5110_SetPixel(x-b, y-a, set);
    }

    if(p < 0)
    {
      p += (3 + 2*a);
      a++;
    }
    else
    {
      p += (5 + 2*(a-b));
      a++;
      b--;
    }
  } while(a <=b);
}

void N5110_DrawBitmap(uint8_t x, uint8_t y, uint8_t * bitmap, uint8_t width, uint8_t height)
{
  uint8_t bit, data;

  for (int cy=0; cy < height; cy++)
  {
    bit= cy % 8;
    for(int cx=0; cx < width; cx++)
    {
      data = bitmap[cx+(cy/8*width)];
      if ((data & (1<<bit)))
      {
        N5110_SetPixel(x+cx, y+cy, 1);
      }
      else
      {
        N5110_SetPixel(x+cx, y+cy, 0);
      }
    }
  }
}
