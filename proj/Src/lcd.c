#include "py32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

void lcd_bk_ctl(int level)
{
	if(level){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	}
}

void lcd_reset_ctl(int level)
{
	if(level){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
	}
}

void lcd_dc_ctl(int level)
{

}

void lcd_init(void)
{

}

