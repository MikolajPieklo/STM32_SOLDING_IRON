/*
 * HD44780.c
 *
 *  Created on: 15.04.2018
 *      Author: Mikolaj
 */

#include <HD44780.h>
#include <delay.h>
#include <gpio.h>

static inline void lcd_sendHalf(uint8_t data);
static inline void lcd_write_byte(uint8_t data);
static inline void lcd_write_cmd(uint8_t cmd);
static inline void lcd_char(uint8_t data);
static inline void lcd_locate(uint8_t x, uint8_t y);
static inline void lcd_str(uint8_t *text);
static inline void lcd_char_XY(uint8_t x, uint8_t y, uint8_t znak);

static inline void lcd_sendHalf(uint8_t data)
{
   LL_GPIO_SetOutputPin(LCD_E_GPIO_Port, LCD_E_Pin); // E High

   if ((data & 0x01) == 0x01) // D4
   {
      LL_GPIO_SetOutputPin(D4_GPIO_Port, D4_Pin);
   }
   else
   {
      LL_GPIO_ResetOutputPin(D4_GPIO_Port, D4_Pin);
   }

   if ((data & 0x02) == 0x02) // D5
   {
      LL_GPIO_SetOutputPin(D5_GPIO_Port, D5_Pin);
   }
   else
   {
      LL_GPIO_ResetOutputPin(D5_GPIO_Port, D5_Pin);
   }

   if ((data & 0x04) == 0x04) // D6
   {
      LL_GPIO_SetOutputPin(D6_GPIO_Port, D6_Pin);
   }
   else
   {
      LL_GPIO_ResetOutputPin(D6_GPIO_Port, D6_Pin);
   }

   if ((data & 0x08) == 0x08) // D7
   {
      LL_GPIO_SetOutputPin(D7_GPIO_Port, D7_Pin);
   }
   else
   {
      LL_GPIO_ResetOutputPin(D7_GPIO_Port, D7_Pin);
   }

   LL_GPIO_ResetOutputPin(LCD_E_GPIO_Port, LCD_E_Pin); // E Low
}

static inline void lcd_write_byte(uint8_t data)
{
   lcd_sendHalf(data >> 4);
   lcd_sendHalf(data);

   TS_Delay_ms(1);
}

static inline void lcd_write_cmd(uint8_t cmd)
{
   LL_GPIO_ResetOutputPin(LCD_RS_GPIO_Port, LCD_RS_Pin); // RS
   lcd_write_byte(cmd);
}

static inline void lcd_char(uint8_t data)
{
   LL_GPIO_SetOutputPin(LCD_RS_GPIO_Port, LCD_RS_Pin); // RS
   lcd_write_byte(data);
}

static inline void lcd_locate(uint8_t x, uint8_t y)
{
   switch (y)
   {
   case 0:
      lcd_write_cmd(LCDC_SET_DDRAM | (LCD_LINE1 + x));
      break;

   case 1:
      lcd_write_cmd(LCDC_SET_DDRAM | (LCD_LINE2 + x));
      break;
   }
}

static inline void lcd_str(uint8_t *text)
{
   while (*text)
   {
      lcd_char(*text++);
   }
}

static inline void lcd_char_XY(uint8_t x, uint8_t y, uint8_t character)
{
   lcd_locate(x, y);
   lcd_char(character);
}

void LCD_Init(void)
{
   LL_GPIO_ResetOutputPin(LCD_E_GPIO_Port, LCD_E_Pin);   // E
   LL_GPIO_ResetOutputPin(LCD_RS_GPIO_Port, LCD_RS_Pin); // RS

   lcd_sendHalf(0x03);
   TS_Delay_ms(5);
   lcd_sendHalf(0x03);
   TS_Delay_ms(1);
   lcd_sendHalf(0x03);
   TS_Delay_ms(1);
   lcd_sendHalf(0x02);
   TS_Delay_ms(1);

   // Juz jestesmy w trybie 4-bitowym. Tutaj dokonujemy ustawien wyswietlacza:
   lcd_write_cmd(LCDC_ENTRY_MODE | LCD_EM_LEFT | LCD_EM_SHIFT_CURSOR);
   // lcd_write_cmd( LCDC_ENTRY_MODE | LCD_EM_LEFT | LCD_EM_SHIFT_CURSOR );
   lcd_write_cmd(LCD_FUNC | LCD_4_BIT | LCDC_TWO_LINE | LCDC_FONT_5x10);
   lcd_write_cmd(LCD_ONOFF | LCD_DISP_ON | LCDC_CURSOR_OFF | LCDC_BLINK_OFF);
   lcd_write_cmd(LCD_SHIFT | LCDC_SHIFT_CURSOR | LCDC_SHIFT_LEFT);

   lcd_write_cmd(LCD_CLEAR);
   TS_Delay_ms(1);

   // CG00
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x00);
   lcd_char(0x00);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x01);
   lcd_char(0x1D);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x02);
   lcd_char(0x15);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x03);
   lcd_char(0x15);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x04);
   lcd_char(0x15);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x05);
   lcd_char(0x15);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x06);
   lcd_char(0x17);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x07);
   lcd_char(0x00);
   // CG01
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x08);
   lcd_char(0x08);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x09);
   lcd_char(0x1C);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x0A);
   lcd_char(0x08);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x0B);
   lcd_char(0x00);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x0C);
   lcd_char(0x00);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x0D);
   lcd_char(0x00);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x0E);
   lcd_char(0x00);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x0F);
   lcd_char(0x00);
   // CG02
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x10);
   lcd_char(0x00);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x11);
   lcd_char(0x00);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x12);
   lcd_char(0x00);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x13);
   lcd_char(0x08);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x14);
   lcd_char(0x08);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x15);
   lcd_char(0x14);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x16);
   lcd_char(0x14);
   lcd_write_cmd(LCDC_SET_CGRAM_ADDR | 0x17);
   lcd_char(0x08);

   LCD_Str_XY(0, 0, "T 000"); /* Target temperature. */
   lcd_char_XY(1, 0, 0x01);
   lcd_char_XY(5, 0, 0xDF);
   lcd_char_XY(6, 0, 'C');
   LCD_Str_XY(9, 0, "NOT CON");
   LCD_Str_XY(0, 1, "T 000"); /* Current temperature. */
   lcd_char_XY(1, 1, 0x02);
   lcd_char_XY(5, 1, 0xDF);
   lcd_char_XY(6, 1, 'C');
   lcd_char_XY(10, 1, 0x00);
   LCD_Str_XY(14, 1, "0%");
}

void LCD_Str_XY(uint8_t x, uint8_t y, char *text)
{
   lcd_locate(x, y);

   while (*text)
   {
      lcd_char(*text++);
   }
}
