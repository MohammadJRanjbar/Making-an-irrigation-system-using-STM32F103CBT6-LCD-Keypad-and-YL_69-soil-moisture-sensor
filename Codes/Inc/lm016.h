
#ifndef _LCD_H
#define _LCD_H

#include "stm32f1xx_hal.h"
#include "main.h"

void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(unsigned char row, unsigned char col);
void lcd_puts(char *str);
void lcd_reset_pins(void);
void lcd_write(unsigned char data);
void lcd_command_write(unsigned char data);
void lcd_putchar(unsigned char data);
void check_LSB_MSB(unsigned char data);
#endif
