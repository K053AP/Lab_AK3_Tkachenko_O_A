#ifndef INC_LCD_I2C_LIB_H_
#define INC_LCD_I2C_LIB_H_

#include <main.h>
#include <string.h>
#include <stdbool.h>

void lcd_Backlight(bool state);
void lcd_Init(void);
void lcd_Clean(void);
void lcd_SetCursor(uint8_t x, uint8_t y);
void lcd_Print_symbol(uint8_t symbol);
void lcd_Print_text(char *message);
void lcd_Clean_Text(void);

#endif /* INC_LCD_I2C_LIB_H_ */
