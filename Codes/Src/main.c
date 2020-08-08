/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lm016.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
int flag_interupt = 0;
int col = -1;
unsigned char keys[16] = {'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};
unsigned char keypad_scan(void); //when interupts come,we will scan rows and reading cols to findout wich key is pressed.
void shift(int i); //shifting 0 in rows.
unsigned char key1; //character returned from keypad_scan
int t_open = 60; //default 1 min.
int Condition_flag=0; //this flag is used to avoid printing wet_menu in while loop.
int minutes;
int seconds;
int second1;
int second2;
int flag_timer = 0; //this flag is used to show that timer has enabled and valve is open.
char t_open2 = '1'; 
int flag_rained = 0; //this flag is used when soil is dry and without watering plant, soid has become wet because of rain. and used to avoid micro to waits only for user to press A.
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

	void Wet_Menue()
	{
		if(Condition_flag==0)
		{
			lcd_clear();
			lcd_puts("Moisture:Wet");
			lcd_set_cursor(2,1); //go to second line
			lcd_puts("Open Time :");
			if(flag_rained == 1) //if rain has come,we print to lcd thats it become wet because of rain not opening valve.
				lcd_puts("rain");
			else
				lcd_putchar(t_open2);
		}
		Condition_flag=1; //this variable is used to avoid printing so many times in while loop when we are in wet condition.
	}

	void Dry_Menue()
	{
			lcd_clear();
			lcd_puts("Moisture:Dry");
			lcd_set_cursor(2,1); //go to second line
			lcd_puts("Press A");
			do{
					if(HAL_GPIO_ReadPin(soil_GPIO_Port,soil_Pin) == 0) //if soil has become wet meanwhile its waiting for user to press A, we just exit loop to go to the wet menu.
					{
						Condition_flag=0;
						flag_rained = 1; //rain has come and valve will not be opened.
						return;
					}	
					while(flag_interupt==0 && HAL_GPIO_ReadPin(soil_GPIO_Port,soil_Pin) == 1); //wait until interupt comes and soil is still dry.
					key1 = keypad_scan(); //find out which key is pressed
					flag_interupt=0; //to stuck in while loop until interupt come
			}while(key1!='A'); //user have to press only 'A'
			
			flag_rained = 0; //valve has opened.
			flag_interupt=0;
			lcd_clear();
			lcd_puts("Choose Time From");
			lcd_set_cursor(2,1);
			lcd_puts("{1,2,3} Min :");
		
			do{
					while(flag_interupt==0 ); //waiting for user to press a key
					flag_interupt=0;
					key1=keypad_scan(); //interupt has come and we can scan keypad.		
					if(key1=='1')
					{
						t_open2 = '1';
						t_open = 60;
						break;
					}
					else if(key1=='2')  //if pressed key is 1 or 2 or 3 exits loop.else it will wait until these keys are pressed 
					{	
						t_open2 = '2';
						t_open = 120;
						break;
					}
					else if(key1=='3')
					{
						t_open2 = '3';
						t_open = 180;
						break;
					}
			}while(1);
			
			lcd_putchar(t_open2);
			HAL_Delay(2000); //just for showing what user has pressed.
			lcd_clear();
			HAL_GPIO_WritePin(valve_GPIO_Port,valve_Pin,GPIO_PIN_SET); //opening valve
			HAL_TIM_Base_Start_IT(&htim2);//starting timer count down.
			Condition_flag=0;
			flag_timer = 1; //timer is enable and we want to print remaining time of watering.
	}
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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	
	lcd_init(); //initializing
	lcd_clear();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		while(flag_timer == 1) //if timer is enable we calculate seconds and minute and show it in lcd.
		{
			char str[4];
			minutes = t_open / 60; 
			seconds = t_open % 60; 
			second1 = seconds / 10;
			second2 = seconds % 10;
			lcd_set_cursor(1,1);
			lcd_puts("Time remaining :");
			lcd_set_cursor(2,6);
			sprintf(str,"0%d:%d%d",minutes,second1,second2);	
			lcd_puts(str);
			
		}
		if(HAL_GPIO_ReadPin(soil_GPIO_Port,soil_Pin) == 1 && flag_timer==0) //if soil is dry
		{
			Dry_Menue();
		}
		else if(HAL_GPIO_ReadPin(soil_GPIO_Port,soil_Pin) == 0 && flag_timer!=1)
		{
			Wet_Menue();
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


unsigned char keypad_scan(void)
{
	int position; //position in keys array.
	
	for (int row = 0; row<4; row++)
	{	
		shift(row); //shifting zero in rows.
		col = -1; //when col is -1 , no key is pressed.
		if(HAL_GPIO_ReadPin(col1_GPIO_Port,col1_Pin) == 0) //reading which column is pressed.
			col = 0;
		if(HAL_GPIO_ReadPin(col2_GPIO_Port,col2_Pin) == 0)
			col = 1;
		if(HAL_GPIO_ReadPin(col3_GPIO_Port,col3_Pin) == 0)
			col = 2;
		if(HAL_GPIO_ReadPin(col4_GPIO_Port,col4_Pin) == 0)
			col = 3;
		if(col != -1) //some key is pressed.
		{
			while(HAL_GPIO_ReadPin(col1_GPIO_Port,col1_Pin) ==0){};
			while(HAL_GPIO_ReadPin(col2_GPIO_Port,col2_Pin) ==0){}; //when user is holding a key,dont do multiple times.
			while(HAL_GPIO_ReadPin(col3_GPIO_Port,col3_Pin) ==0){};
			while(HAL_GPIO_ReadPin(col4_GPIO_Port,col4_Pin) ==0){};
			position = 4 * row + col; //finding index pressed key in array with variable row and col.
			col = -1; 
			HAL_GPIO_WritePin(row1_GPIO_Port,row1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(row2_GPIO_Port,row2_Pin,GPIO_PIN_RESET); //reseting pins to initial values.
			HAL_GPIO_WritePin(row3_GPIO_Port,row3_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(row4_GPIO_Port,row4_Pin,GPIO_PIN_RESET);						
			return keys[position];
		}
		HAL_Delay(50); //for debouncing,we will add delay.
	}
			HAL_GPIO_WritePin(row1_GPIO_Port,row1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(row2_GPIO_Port,row2_Pin,GPIO_PIN_RESET); 
			HAL_GPIO_WritePin(row3_GPIO_Port,row3_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(row4_GPIO_Port,row4_Pin,GPIO_PIN_RESET);	
	//return '0';
}

void shift(int i)
{	
	unsigned char pins[4] = {0x7,0xB,0xD,0xE};
	HAL_GPIO_WritePin(row1_GPIO_Port,row1_Pin,(GPIO_PinState) (pins[i] & 0x8)); //if output of (pins[i] & 0x8) is anything excpet 0x00 it will cast to GPIO_PIN_SET
	HAL_GPIO_WritePin(row2_GPIO_Port,row2_Pin,(GPIO_PinState) (pins[i] & 0x4));
	HAL_GPIO_WritePin(row3_GPIO_Port,row3_Pin,(GPIO_PinState) (pins[i] & 0x2));
	HAL_GPIO_WritePin(row4_GPIO_Port,row4_Pin,(GPIO_PinState) (pins[i] & 0x1));
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	flag_interupt= 1; //to find out that interupts is triggered because of pressing keys.
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance ==  TIM2)
	{
		t_open--;
		if(t_open==0)
		{
				HAL_GPIO_WritePin(valve_GPIO_Port,valve_Pin,GPIO_PIN_RESET); //closing valve.
				flag_timer = 0; //show that timer is stopped.
				HAL_TIM_Base_Stop_IT(&htim2); //stop timer
		}
	}
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
