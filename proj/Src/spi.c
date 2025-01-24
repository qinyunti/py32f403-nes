#include "uart.h"
#include "fifo.h"

#include "py32f4xx_hal.h"
#include "py32f403_ll_spi.h"
#include "FreeRTOS.h"
#include "semphr.h"

SemaphoreHandle_t s_spi_mutex[2];

void spi_init(int id, uint32_t baud, int mode)
{
	s_spi_mutex[id] = xSemaphoreCreateRecursiveMutex();
	if(id == 0){		
		/* PA15  NSS1   FLASH   APB2
		 * PB3   SCK  AF3
		 * PB4   MISO AF3
		 * PB5   MOSI AF3
		 * PC6   NSS2   TOUCH
		 */
			GPIO_InitTypeDef  GPIO_InitStruct;
			__HAL_RCC_GPIOA_CLK_ENABLE();          
			__HAL_RCC_GPIOB_CLK_ENABLE();   
			__HAL_RCC_GPIOC_CLK_ENABLE();   
			__HAL_RCC_SPI1_CLK_ENABLE();
		
			GPIO_InitStruct.Pin = GPIO_PIN_15;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;              
			GPIO_InitStruct.Pull = GPIO_PULLUP;                    
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;    
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);  
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
			GPIO_InitStruct.Pin = GPIO_PIN_6; 
			HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET); 
			
			GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;              
			GPIO_InitStruct.Pull = GPIO_PULLUP;                    
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;    
			GPIO_InitStruct.Alternate = GPIO_AF3_SPI1;   
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);  
		
			SPI_HandleTypeDef hspi;
			hspi.Instance = SPI1;
			uint32_t clk = HAL_RCC_GetPCLK2Freq();
			uint32_t div = clk/baud;
			if(div<=2){
				hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
			}else if(div<=4){
				hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
			}else if(div<=8){
				hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
			}else if(div<=16){
				hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
			}else if(div<=32){
				hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
			}else if(div<=64){
				hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
			}else if(div<=128){
				hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
			}else{
				hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
			}
			if(mode & 0x01){
				hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
			}else{
				hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
			}
			if(mode & 0x02){
				hspi.Init.CLKPolarity = SPI_POLARITY_HIGH;
			}else{
				hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
			}
			hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
			hspi.Init.CRCPolynomial = 0;
			hspi.Init.DataSize = SPI_DATASIZE_8BIT;
			hspi.Init.Direction = SPI_DIRECTION_2LINES;
			hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
			hspi.Init.Mode = SPI_MODE_MASTER;
			HAL_SPI_Init(&hspi);
			LL_SPI_Enable(SPI1);
	}else if(id == 1){
	
	
	}
}


uint32_t spi_trans(int id, int cs, uint8_t* tx_buffer, uint8_t* rx_buffer, uint32_t len, int flag)
{
	uint8_t rx;
	
	if(pdPASS != xSemaphoreTakeRecursive(s_spi_mutex[id], 100)){
		return 0;
	}
	
	if(id == 0){
		if(cs == 0){
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
		}else if(cs == 1){
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET); 
		}

		for(uint32_t i=0; i<len; i++){
			while(LL_SPI_IsActiveFlag_TXE(SPI1) == 0);
			if(tx_buffer != (uint8_t*)0){
				LL_SPI_TransmitData8(SPI1, tx_buffer[i]);
			}else{
				LL_SPI_TransmitData8(SPI1, 0xFF);
			}
			while(LL_SPI_IsActiveFlag_RXNE(SPI1) == 0);
			rx = LL_SPI_ReceiveData8(SPI1);
			if(rx_buffer != (uint8_t*)0){
				rx_buffer[i] = rx;
			}
		}
		
		if(flag){
			if(cs == 0){
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
			}else if(cs == 1){
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET); 
			}
		}
	}else if(id == 1){
	
	
	}

	xSemaphoreGiveRecursive(s_spi_mutex[id]);
	return len;
}

