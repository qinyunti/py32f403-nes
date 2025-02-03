
#include <stdio.h>
#include <stdlib.h>
#include "st7789_itf.h"
#include "st7789_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "xprintf.h"

static void rgb_test(void)
{
    for(int x=0;x<240;x++)
    {
        for(int y=0;y<320;y++)
        {
            st7789_itf_set_pixel(x, y, 0xF800);
        }
    }
    st7789_itf_sync();
		vTaskDelay((1000*1000/configTICK_RATE_HZ));

    for(int x=0;x<240;x++)
    {
        for(int y=0;y<320;y++)
        {
            st7789_itf_set_pixel(x, y, 0x07E0);
        }
    }
    st7789_itf_sync();
		vTaskDelay((1*1000/configTICK_RATE_HZ));

    for(int x=0;x<240;x++)
    {
        for(int y=0;y<320;y++)
        {
            st7789_itf_set_pixel(x, y, 0x001F);
        }
    }
    st7789_itf_sync();
		vTaskDelay((1*1000/configTICK_RATE_HZ));
}

static void rgb_test_direct(void)
{
    for(int x=0;x<240;x++)
    {
        for(int y=0;y<320;y++)
        {
            st7789_itf_set_pixel_direct(x, y, 0xF800);
        }
    }
		vTaskDelay((1000*1000/configTICK_RATE_HZ));

    for(int x=0;x<240;x++)
    {
        for(int y=0;y<320;y++)
        {
            st7789_itf_set_pixel_direct(x, y, 0x07E0);
        }
    }
		vTaskDelay((1000*1000/configTICK_RATE_HZ));

    for(int x=0;x<240;x++)
    {
        for(int y=0;y<320;y++)
        {
            st7789_itf_set_pixel_direct(x, y, 0x001F);
        }
    }
		vTaskDelay((1000*1000/configTICK_RATE_HZ));
}

int st7789_test(void)
{
    st7789_itf_init();
    rgb_test();
    uint32_t start;
    uint32_t end;
    uint32_t ftime = 0;
    while(1)
    {
        start = xTaskGetTickCount();
        for(int i=0;i<100;i++)
        {
            rgb_test_direct();
        }
        end = xTaskGetTickCount();
        ftime = (end - start);

        uint32_t fps = (ftime*2+100)/(100*2);  /* 刷新一次的时间uS */
        if(fps > 0)
        {
            xprintf("FPS:%d\r\n",1000000/fps);
        }
        else
        {
            xprintf("FPS:%d\r\n",0);
        }
    }
}