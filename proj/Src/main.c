#include <string.h>
#include "py32f4xx_hal.h"
#include "py32f403_hal_rcc.h"

#include "uart.h"
#include "spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "xprintf.h"
#include "shell.h"
#include "shell_func.h"
#include "flash_itf.h"
#include "ff.h"


static FATFS fs;             /* Filesystem object */
static BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */

static void fs_init(void)
{
    FRESULT res;        /* API result code */
		if(FR_OK != (res = f_mount (&fs, "0:", 1)))
	  {
			xprintf("mount err %d, mkfs\r\n",res);
			res = f_mkfs("0:",0,work,sizeof(work));
			if(res == 0)
			{
				xprintf("mkfs ok\r\n");
				if(FR_OK == f_mount (&fs, "0:", 1))
				{
					xprintf("mount ok\r\n");
				}
				else
				{
					xprintf("mount err\r\n");
				}
			}
			else
		  {
				xprintf("mkfs err %d\r\n",res);
			}
		}
		else
		{
			xprintf("mount ok\r\n");
		} 
}


void app_fatfs_mkfs(char* path)
{
    FRESULT res;        /* API result code */
		f_unmount(path);
    res = f_mkfs("2:",0,work,sizeof(work));
    if(res == 0)
    {
			xprintf("mkfs ok\r\n");
			if(FR_OK == f_mount (&fs, path, 1))
			{
				xprintf("mount ok\r\n");
			}
			else
			{
				xprintf("mount err %d\r\n",res);
			}
    }
    else
    {
			xprintf("mkfs err %d\r\n",res);
    }
}


void app_fatfs_mount(char* path)
{
    FRESULT res;        /* API result code */
		res = f_unmount(path);

		if(FR_OK == (res = f_mount (&fs, path, 1)))
		{
				xprintf("mount ok\r\n");
		}
		else
		{
				xprintf("mount err %d\r\n",res);
		}
}

static void clk_out(void)
{
	/* PA8 AF0 MCO*/
	GPIO_InitTypeDef  GPIO_InitStruct;
	__HAL_RCC_GPIOA_CLK_ENABLE();                   

	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;            
	GPIO_InitStruct.Pull = GPIO_PULLUP;                 
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;         
	GPIO_InitStruct.Alternate = GPIO_AF0_MCO;        
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);  

	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);
	
}

static void clk_init(void)
{
	RCC_OscInitTypeDef  RCC_OscInitStruct;
	RCC_OscInitStruct.HSEFreq = RCC_HSE_16_32MHz;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSI48MState = RCC_HSI48M_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.LSEDriver = RCC_LSEDRIVE_HIGH;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	
	RCC_ClkInitTypeDef  RCC_ClkInitStruct;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, 6);
	
	SystemCoreClockUpdate();
}

static void xprintf_out_port(int ch)
{
    uint8_t val = ch;
    uart_send(0,&val,1);
}

static int xprintf_in_port(void)
{
    uint32_t len;
    uint8_t val;
    do
    {
        len = uart_read(0,&val,1);
    }while(len == 0);
    return val;
}


static void shell_task(void *arg)
{   
    (void)arg;
	  xprintf("shell task start.\r\n"); 
	  fs_init();
    while(1)
    {
        shell_exec();
        vTaskDelay((1*1000/configTICK_RATE_HZ));
    }
}

uint32_t shell_read(uint8_t *buff, uint32_t len)
{
	return uart_read(0,buff,len);
}

void shell_write(uint8_t *buff, uint32_t len)
{
	uart_send(0,buff,len);
}

static void shell_task_init(void)
{
		TaskHandle_t xHandle;
    xdev_out(xprintf_out_port);
    xdev_in(xprintf_in_port);
    shell_set_itf(shell_read, shell_write, (shell_cmd_cfg*)g_shell_cmd_list_ast, 1);
    /* 创建任务,执行shell */
	  xTaskCreate( shell_task, "shell", 512, NULL, 1, &xHandle ); 
}

int main(void)
{
	HAL_Init();
	clk_init();
	clk_out();
	uart_init(0,115200);
	uart_init(1,115200);
	spi_init(0, 36000000ul, 0);
	flash_itf_init(); 

	shell_task_init();
	
	vTaskStartScheduler();
}