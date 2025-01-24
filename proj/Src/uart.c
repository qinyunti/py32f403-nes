#include "uart.h"
#include "fifo.h"

#include "py32f4xx_hal.h"
#include "py32f403_ll_usart.h"

#define CriticalAlloc()
#define EnterCritical()	__disable_irq()
#define ExitCritical()  __enable_irq()

uint8_t s_uart_rx_buffer[2][128];

static fifo_st s_uart_fifo_dev[2]=
{
	{
	 .in = 0,
	 .len = 0,
	 .out = 0,
	 .buffer = s_uart_rx_buffer[0],
	 .buffer_len = sizeof(s_uart_rx_buffer[0]),
	},
	{
	 .in = 0,
	 .len = 0,
	 .out = 0,
	 .buffer = s_uart_rx_buffer[1],
	 .buffer_len = sizeof(s_uart_rx_buffer[1]),
	}
};

void uart_rx_cb(int id, uint8_t* buff, uint32_t len)
{
	fifo_in(&(s_uart_fifo_dev[id]), buff, len);
}

void USART1_IRQHandler(void)
{
	uint8_t ch;
	ch = LL_USART_ReceiveData8(USART1);
	uart_rx_cb(0, &ch, 1);
}

void USART3_IRQHandler(void)
{
	uint8_t ch;
	ch = LL_USART_ReceiveData8(USART3);
	uart_rx_cb(1, &ch, 1);
}

void uart_init(int id, uint32_t baud)
{
	if(id == 0){
	/* PA9  USART1_TX AF2
	 * PA10 USART1_RX AF2
	 */
		UART_HandleTypeDef huart;
		GPIO_InitTypeDef  GPIO_InitStruct;

		__HAL_RCC_GPIOA_CLK_ENABLE();          
		__HAL_RCC_USART1_CLK_ENABLE();
		
		GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;              
		GPIO_InitStruct.Pull = GPIO_PULLUP;                    
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;    
		GPIO_InitStruct.Alternate = GPIO_AF2_USART1;   
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);  
		 
		huart.Instance = USART1;
		huart.Init.BaudRate = baud;
		huart.Init.WordLength = UART_WORDLENGTH_8B;
		huart.Init.StopBits = UART_STOPBITS_1;
		huart.Init.Parity = UART_PARITY_NONE;
		huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart.Init.Mode = UART_MODE_TX_RX;
		huart.Init.OverSampling = UART_OVERSAMPLING_8;
		HAL_UART_Init(&huart);
		 
		__HAL_UART_ENABLE_IT(&huart, UART_IT_RXNE);
		HAL_NVIC_SetPriority(USART1_IRQn,2,0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	}else if(id == 1){
	/* PB10 USART3_TX AF2
	 * PB11 USART3_RX AF2
	 */
		UART_HandleTypeDef huart;
		GPIO_InitTypeDef  GPIO_InitStruct;

		__HAL_RCC_GPIOB_CLK_ENABLE();          
		__HAL_RCC_USART3_CLK_ENABLE();
		
		GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;              
		GPIO_InitStruct.Pull = GPIO_PULLUP;                    
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;    
		GPIO_InitStruct.Alternate = GPIO_AF2_USART3;   
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);  
		 
		huart.Instance = USART3;
		huart.Init.BaudRate = baud;
		huart.Init.WordLength = UART_WORDLENGTH_8B;
		huart.Init.StopBits = UART_STOPBITS_1;
		huart.Init.Parity = UART_PARITY_NONE;
		huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart.Init.Mode = UART_MODE_TX_RX;
		huart.Init.OverSampling = UART_OVERSAMPLING_8;
		HAL_UART_Init(&huart);
		 
		__HAL_UART_ENABLE_IT(&huart, UART_IT_RXNE);
		HAL_NVIC_SetPriority(USART3_IRQn,2,0);
		HAL_NVIC_EnableIRQ(USART3_IRQn);
	}
}

uint32_t uart_send(int id, uint8_t* buffer, uint32_t len)
{
	  if(id == 0){
			for(uint32_t i=0;i<len;i++)
			{
				while(LL_USART_IsActiveFlag_TXE(USART1) == 0);
				LL_USART_TransmitData8(USART1,buffer[i]);
			}
		}else if(id == 1){
			for(uint32_t i=0;i<len;i++)
			{
				while(LL_USART_IsActiveFlag_TXE(USART3) == 0);
				LL_USART_TransmitData8(USART3,buffer[i]);
			}
		}
    return len;
}

uint32_t uart_read(int id, uint8_t* buffer, uint32_t len)
{
    uint32_t rlen;
    CriticalAlloc();
    EnterCritical();
    rlen = fifo_out(&(s_uart_fifo_dev[id]), buffer, len);
    ExitCritical();
    return rlen;
}
