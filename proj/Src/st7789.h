#ifndef ST7789_H
#define ST7789_H

#ifdef __cplusplus
    extern "C"{
#endif

#include <stdint.h>

#define ST7789_CHECK_PARAM 1

#define ST7789_HSIZE 240
#define ST7789_VSIZE 320

typedef void    (*st7789_set_dcx_pf)(uint8_t val);                          /**< DCX引脚操作接口,val=1为数据和参数, val=0为命令   */
typedef void    (*st7789_set_reset_pf)(uint8_t val);                        /**< 复位引脚操作,val=1输出高,val=0输出低            */
typedef void    (*st7789_spi_write_pf)(uint8_t* buffer, uint32_t len);      /**< MOSI写接口接口,buffer为待写数据,len为待写长度    */
typedef void    (*st7789_spi_enable_pf)(uint8_t val);                       /**< 使能接口                                       */
typedef void    (*st7789_spi_delay_ms_pf)(uint32_t t);                      /**< 延时接口                                       */
typedef void    (*st7789_init_pf)(void);                                    /**< 初始化接口                                     */
typedef void    (*st7789_deinit_pf)(void);                                  /**< 解除初始化接口                                  */


#define ST7789_CMD_SLPOUT 0x11
#define ST7789_CMD_NORON  0x13
#define ST7789_CMD_INVOFF 0x20
#define ST7789_CMD_INVON 0x21
#define ST7789_CMD_DISPON 0x29
#define ST7789_CMD_CASET  0x2A
#define ST7789_CMD_RASET  0x2B
#define ST7789_CMD_RAMWR  0x2C
#define ST7789_CMD_MADCTL 0x36
#define ST7789_CMD_COLMOD 0x3A
#define ST7789_CMD_PORCTRL 0xB2
#define ST7789_CMD_GCTRL   0xB7
#define ST7789_CMD_VCOMS   0xBB
#define ST7789_CMD_LCMCTRL 0xC0
#define ST7789_CMD_VDVVRHEN  0xC2
#define ST7789_CMD_VRHS 0xC3
#define ST7789_CMD_VDVS 0xC4
#define ST7789_CMD_FRCTRL2 0XC6
#define ST7789_CMD_PWCTRL1 0xD0
#define ST7789_CMD_PVGAMCTRL 0xE0
#define ST7789_CMD_NVGAMCTRL 0xE1

/**
 * \struct st7789_dev_st
 * 设备接口结构体
*/
typedef struct
{
    st7789_set_dcx_pf    set_dcx;      /**< DCX写接口        */
    st7789_set_reset_pf  set_reset;    /**< RESET写接口      */
    st7789_spi_write_pf  write;        /**< 数据写接口       */
    st7789_spi_enable_pf enable;       /**< 使能接口         */
    st7789_spi_delay_ms_pf delay;      /**< 延时接口         */
    st7789_init_pf       init;         /**< 初始化接口       */
    st7789_deinit_pf     deinit;       /**< 解除初始化接口   */

    uint16_t*            buffer;       /**< 显存,用户分配    */        
} st7789_dev_st;

/**
 * \fn st7789_sync
 * 现存写入st7789
 * \param[in] dev \ref st7789_dev_st
 * \paran[in] x0 列开始地址
 * \paran[in] x1 列结束地址
 * \paran[in] y0 行开始地址
 * \paran[in] y1 行结束地址 
 * \paran[in] buffer 待写入数据 
 * \paran[in] len 待写入数据长度 
 * \retval 0 成功
 * \retval 其他值 失败
*/
int st7789_sync(st7789_dev_st* dev, uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1, uint16_t* buffer, uint32_t len);

/**
 * \fn st7789_init
 * 初始化
 * \param[in] dev \ref st7789_dev_st
 * \retval 0 成功
 * \retval 其他值 失败
*/
int st7789_init(st7789_dev_st* dev);

/**
 * \fn st7789_deinit
 * 解除初始化
 * \param[in] dev \ref st7789_dev_st
 * \return 总是返回0
*/
int st7789_deinit(st7789_dev_st* dev);

#ifdef __cplusplus
    }
#endif

#endif