#include "uart.h"
#include "xprintf.h"

static uint8_t s_key_buffer[4];
static int s_key_buffer_len = 0;
static int s_key = 0xFF;
uint8_t key_get(void)
{
	static uint8_t s_pre_key = 0xFF;
	uint8_t tmp;
	uint8_t len;
	do{
		len = uart_read(1,&tmp,1);
			s_key_buffer[s_key_buffer_len] = tmp;
			s_key_buffer_len++;
			if(s_key_buffer_len >= 4){
				if((s_key_buffer[0]==0xAA) && (s_key_buffer[1]==0x55) && ((s_key_buffer[2]+s_key_buffer[3])==(uint8_t)255)){
					s_key = s_key_buffer[2];
					s_key_buffer_len = 0;
				}else{
				  /* 错误帧,丢掉第一个字节 */
					s_key_buffer[0] = s_key_buffer[1];
					s_key_buffer[1] = s_key_buffer[2];
					s_key_buffer[2] = s_key_buffer[3];
					s_key_buffer_len = 3;
					s_key = 0xFF;
				}
			
			}else{
				/* 不足帧长 */
			}
	}while(len > 0);
	if(s_pre_key != s_key){
		s_pre_key = s_key;
		xprintf("key:%02x\r\n",s_key);
	}
	return s_key;
}