
#include "lm016.h"
int counter = 0;
void check_LSB_MSB(unsigned char data)
{
	HAL_GPIO_WritePin(LCD_D7_GPIO_Port,LCD_D7_Pin,(GPIO_PinState) (( (data & 0x08) == 0x08))); //writing lsb or msb parts of data to lcd ports.
	HAL_GPIO_WritePin(LCD_D6_GPIO_Port,LCD_D6_Pin,(GPIO_PinState) (( (data & 0x04) == 0x04)));
	HAL_GPIO_WritePin(LCD_D5_GPIO_Port,LCD_D5_Pin,(GPIO_PinState) (( (data & 0x02) == 0x02)));
	HAL_GPIO_WritePin(LCD_D4_GPIO_Port,LCD_D4_Pin,(GPIO_PinState) (( (data & 0x01) == 0x01)));
}

void reset_pins(void) //reseting lcd pins
{
   HAL_GPIO_WritePin(LCD_D4_GPIO_Port,LCD_D4_Pin,GPIO_PIN_RESET);
   HAL_GPIO_WritePin(LCD_D5_GPIO_Port,LCD_D5_Pin,GPIO_PIN_RESET);
   HAL_GPIO_WritePin(LCD_D6_GPIO_Port,LCD_D6_Pin,GPIO_PIN_RESET);
   HAL_GPIO_WritePin(LCD_D7_GPIO_Port,LCD_D7_Pin,GPIO_PIN_RESET);
}
void lcd_write(unsigned char data)
{
   unsigned char LSB = data & 0x0F;
	unsigned char MSB = (data>>4) & 0x0F; //shift four bits to replace MSB part with LSB.
	
  HAL_Delay(1);

  HAL_GPIO_WritePin(LCD_E_GPIO_Port,LCD_E_Pin,GPIO_PIN_SET); //setting enable port
  reset_pins(); //reseting lcd ports.

  check_LSB_MSB(MSB); //writing MSB part first.
	
  HAL_GPIO_WritePin(LCD_E_GPIO_Port,LCD_E_Pin,GPIO_PIN_RESET); //reseting enable.
  HAL_GPIO_WritePin(LCD_E_GPIO_Port,LCD_E_Pin,GPIO_PIN_SET); //setting enable again to write another command or data.
  
  reset_pins();
	
  check_LSB_MSB(LSB); //now writing LSB part
  
  HAL_GPIO_WritePin(LCD_E_GPIO_Port,LCD_E_Pin,GPIO_PIN_RESET); //reseting enable pin.
}


void lcd_write_command(unsigned char data)
{
   HAL_GPIO_WritePin(LCD_RS_GPIO_Port,LCD_RS_Pin,GPIO_PIN_RESET); //resetting rs so that lcd know that we are sending data.
   lcd_write(data);
}

void lcd_init(void)
{
	counter = 0;
  HAL_Delay(1);
	lcd_clear();
  lcd_write_command(0x28);  //function test : 4 bit mode,2 lines , 5*8 dot.
  lcd_write_command(0x06);  //Entry mode set : Increamental, shifts the entire lcd to the right.
  lcd_write_command(0x0c);  //Display control : display on, cursor off,blink cursor off.
}

void lcd_puts(char *str)
{
  HAL_Delay(1);
  while(*str != '\0') //while pointer hasn't reached end of string.
  {
    lcd_putchar(*str); //use putchar to send to lcd.
    str++;
  }
}

void lcd_set_cursor(unsigned char row, unsigned char col)
{
		HAL_Delay(1); 
    switch(row){
    case 1:
		{
      lcd_write_command( 0x80 + col-1 ); //setting DDRAM address according to first address of each line. for line 1 address is 0x00 and for line 2 address is 0x40;
			counter = col-1;
			break;
		}
    case 2:
		{
			lcd_write_command( 0xC0 + col-1 ); //second line address + column
			counter = 16 + col-1 ;
      break;
		}
  }
}

void lcd_clear(void)
{
  HAL_Delay(1);
	counter=0;
  lcd_write_command(0x01); //clearing display
  lcd_write_command(0x02); //return home
	
}

void lcd_putchar(unsigned char data)
{
		counter++;
		HAL_GPIO_WritePin(LCD_RS_GPIO_Port,LCD_RS_Pin,GPIO_PIN_SET); //setting rs so that lcd know that we are sending data.
		if(counter < 16) //line 1 is not full yet.
				lcd_write(data);
		else if(counter==16) //line 1 is full and we have to go to second line.
		{
				lcd_write(data);
				lcd_set_cursor(2,1);
		}
			else if ( counter > 16 && counter < 32) //line 1 is not full yet.
			{
				lcd_write(data);
			}
			else //line 2 is full and we have to go to first line.
			{
				lcd_write(data);
				lcd_set_cursor(1,1);
				counter = 0;
			}
	}
