#include "py32f4xx_hal.h"
#include "st7789_itf.h"
#include "st7789.h"
#include "spi.h"
#include "FreeRTOS.h"
#include "task.h"

static void port_st7789_set_dcx(uint8_t val)
{
	if(val){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
	}
}                

static void port_st7789_set_bk(uint8_t val)
{
	if(val){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	}
}


static void port_st7789_set_reset(uint8_t val)
{
	if(val){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
	}
}

static void port_st7789_spi_write(uint8_t* buffer, uint32_t len)
{
	spi_trans(1,0,buffer,0,len,1);
}    

static void port_st7789_spi_enable(uint8_t val)
{
	(void)val;
}    

static void port_st7789_delay_ms(uint32_t t)
{
	vTaskDelay((t*1000/configTICK_RATE_HZ));
}

static void port_st7789_init(void)
{
	/* DC PC0 
	 * RESET PC1
	 * BK PA8
	 */
	GPIO_InitTypeDef  GPIO_InitStruct;
	__HAL_RCC_GPIOC_CLK_ENABLE();   
	__HAL_RCC_GPIOA_CLK_ENABLE();   	
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;              
	GPIO_InitStruct.Pull = GPIO_PULLUP;                    
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;    
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);  
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;              
	GPIO_InitStruct.Pull = GPIO_PULLUP;                    
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;    
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);  
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	
	port_st7789_set_bk(0);
	port_st7789_set_dcx(1);
	
	port_st7789_set_reset(0);
	vTaskDelay((100*1000/configTICK_RATE_HZ));
	port_st7789_set_reset(1);
	vTaskDelay((100*1000/configTICK_RATE_HZ));

	port_st7789_set_bk(1);
}           

static void port_st7789_deinit(void)
{

}        
    
/******************************************************************************
 *                        以下是ST7789设备实例
 * 
******************************************************************************/

/* 设备实例 */
static st7789_dev_st s_st7789_itf_dev =
{
    .set_dcx = port_st7789_set_dcx,
    .set_reset = port_st7789_set_reset,
    .write = port_st7789_spi_write,
    .enable = port_st7789_spi_enable,
    .delay = port_st7789_delay_ms,
    .init = port_st7789_init,
    .deinit = port_st7789_deinit,

    .buffer = (uint16_t*)0,
};

/******************************************************************************
 *                        以下是对外操作接口
 * 
******************************************************************************/


/**
 * \fn st7789_itf_init
 * 初始化
 * \retval 0 成功
 * \retval 其他值 失败
*/
int st7789_itf_init(void)
{
    return st7789_init(&s_st7789_itf_dev);
}

/**
 * \fn st7789_itf_deinit
 * 解除初始化
 * \retval 0 成功
 * \retval 其他值 失败
*/
int st7789_itf_deinit(void)
{
    return st7789_deinit(&s_st7789_itf_dev);
}

/**
 * \fn st7789_itf_sync
 * 刷新显示
 * \retval 0 成功
 * \retval 其他值 失败
*/
int st7789_itf_sync(void)
{
    return st7789_sync(&s_st7789_itf_dev, 0, ST7789_HSIZE-1, 0, ST7789_VSIZE-1, s_st7789_itf_dev.buffer, ST7789_HSIZE*ST7789_VSIZE*2);
}

/**
 * \fn st7789_itf_set_pixel
 * 写点
 * \param[in] x x坐标位置
 * \param[in] y y坐标位置
 * \param[in] rgb565 颜色
*/
void st7789_itf_set_pixel(uint16_t x, uint16_t y, uint16_t rgb565)
{
    //if(x >= ST7789_HSIZE)
    //{
    //    return -1;
    //}
    //if(y >= ST7789_VSIZE)
    //{
    //    return -1;
    //}
    s_st7789_itf_dev.buffer[y*ST7789_HSIZE + x] = (uint16_t)((rgb565>>8)&0xFF) | (uint16_t)((rgb565<<8) & 0xFF00);
}

/**
 * \fn st7789_itf_set_pixel_0
 * 写点
 * \param[in] offset 偏移位置
 * \param[in] rgb565 颜色
*/
void st7789_itf_set_pixel_0(uint32_t offset, uint16_t rgb565)
{
    s_st7789_itf_dev.buffer[offset] = (uint16_t)((rgb565>>8)&0xFF) | (uint16_t)((rgb565<<8) & 0xFF00);
}

/**
 * \fn st7789_itf_get_pixel
 * 读点
 * \param[in] x x坐标位置
 * \param[in] y y坐标位置
 * \return rgb565颜色
*/
uint16_t st7789_itf_get_pixel(uint16_t x, uint16_t y)
{
    uint16_t color = s_st7789_itf_dev.buffer[y*ST7789_HSIZE + x]; 
    return ((uint16_t)(color>>8) | (uint16_t)(color<<8));
}

/**
 * \fn st7789_itf_fill_direct
 * 直接填充区域
 * \param[in] x x开始坐标位置
 * \param[in] w 宽度
 * \param[in] y y开始坐标位置
 * \param[in] h 高度
 * \param[in] buffer rgb565颜色缓存区
*/
void st7789_itf_fill_direct(uint16_t x, uint16_t w, uint16_t y, uint16_t h, uint16_t* buffer)
{
		st7789_sync(&s_st7789_itf_dev, x, x+w-1, y, y+h-1, buffer, w*h*2);
}

/**
 * \fn st7789_itf_set_pixel_direct
 * 直接写点
 * \param[in] x x坐标位置
 * \param[in] y y坐标位置
 * \param[in] rgb565 颜色
*/
void st7789_itf_set_pixel_direct(uint16_t x, uint16_t y, uint16_t rgb565)
{
		uint16_t tmp = rgb565;
		st7789_sync(&s_st7789_itf_dev, x, x, y, y, &tmp, 2);
}
