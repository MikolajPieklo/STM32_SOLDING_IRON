/*
 * HD44780.h
 *
 *  Created on: 15.04.2018
 *      Author: Mikolaj
 */
//#include "stm32f1xx_hal_gpio.h"

#include <stdint.h>

#ifndef HD44780_H_
#define HD44780_H_

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_CLEAR 0x01
#define LCD_HOME 0x02

#define LCDC_ENTRY_MODE 0x04
#define LCD_EM_RIGHT 0x02
#define LCD_EM_LEFT 0x00
#define LCD_EM_SHIFT_DISPLAY 0x01
#define LCD_EM_SHIFT_CURSOR 0x00

#define LCD_ONOFF 0x08
#define LCD_DISP_ON 0x04
#define LCD_DISP_OFF 0x00
#define LCDC_CURSOR_ON 0x02
#define LCDC_CURSOR_OFF 0x00
#define LCDC_BLINK_ON 0x01
#define LCDC_BLINK_OFF 0x00

#define LCD_SHIFT 0x10
#define LCDC_SHIFT_DISP 0x08
#define LCDC_SHIFT_CURSOR 0x00
#define LCDC_SHIFT_RIGHT 0x04
#define LCDC_SHIFT_LEFT 0x00

#define LCD_FUNC 0x20
#define LCD_8_BIT 0x10
#define LCD_4_BIT 0x00
#define LCDC_TWO_LINE 0x08
#define LCDC_FONT_5x10 0x04
#define LCDC_FONT_5x7 0x00
#define LCDC_SET_CGRAM_ADDR 0x40

#define LCDC_SET_DDRAM 0x80

#define LCD_LINE1 0x00
#define LCD_LINE2 0x40

void LCD_Init (void);
void LCD_Str_XY (uint8_t x, uint8_t y, char *text);


#ifdef __cplusplus
}
#endif

#endif /* HD44780_H_ */
