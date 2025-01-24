#include <stdio.h>
#include "shell.h"
#include "shell_func.h"
#include "xprintf.h"
#include "spi.h"
#include "FreeRTOS.h"
#include "flash_itf.h"
#include "ff.h"

static void helpfunc(uint8_t* param);
static void spirxfunc(uint8_t* param);
static void spitxrxfunc(uint8_t* param);
static void printflashfunc(uint8_t* param);
static void writeflashfunc(uint8_t* param);
static void printfilefunc(uint8_t* param);
static void writefilefunc(uint8_t* param);

/**
 * 最后一行必须为0,用于结束判断
*/
const shell_cmd_cfg g_shell_cmd_list_ast[ ] = 
{
  { (uint8_t*)"help",         helpfunc,         (uint8_t*)"help"}, 
  { (uint8_t*)"spirx",        spirxfunc,        (uint8_t*)"spirx id cs len flag"}, 		
  { (uint8_t*)"spitxrx",      spitxrxfunc,      (uint8_t*)"spirx id cs hexstr flag"},
  { (uint8_t*)"printflash",   printflashfunc,   (uint8_t*)"printflash addr[hex] len"},
  { (uint8_t*)"writeflash",   writeflashfunc,   (uint8_t*)"writeflash addr[hex] hexstr"}, 	
  { (uint8_t*)"printfile",    printfilefunc,    (uint8_t*)"printfile path addr size"},  
  { (uint8_t*)"writefile",    writefilefunc,    (uint8_t*)"writefile path addr[hex] hexstr"}, 
  { (uint8_t*)0,		          0 ,               0},
};


static int ascii2uc(const char c, unsigned char *uc)
{
    if ((c >= '0') && (c <= '9')) {
        *uc = c - '0';
    } else if ((c >= 'a') && (c <= 'f')) {
        *uc = c - 'a' + 10;
    } else if ((c >= 'A') && (c <= 'F')) {
        *uc = c - 'A' + 10;
    } else {
        return -1;
    }

    return 0;
}

static uint32_t str2hex(const char* str, unsigned char *buff, uint32_t len)
{
  uint32_t num = 0;
  uint8_t hex = 0;
  while(1)
  {
    uint8_t tmp1;
    uint8_t tmp2;
    if(ascii2uc(*str++, &tmp1) < 0)
    {
      break;
    }
    if(ascii2uc(*str++, &tmp2) < 0)
    {
      break;
    }
    hex = tmp1*16 + tmp2;
    *buff++ = hex;
    num++;
    if(num >= len)
    {
      break;
    }
  }
  return num;
}

static void helpfunc(uint8_t* param)
{
	  (void)param;
    unsigned int i;
    xprintf("\r\n");
    xprintf("**************\r\n");
    xprintf("*   SHELL    *\r\n");
    xprintf("*   V1.0     *\r\n");
    xprintf("**************\r\n");
    xprintf("\r\n");
    for (i=0; g_shell_cmd_list_ast[i].name != 0; i++)
    {
        xprintf("%02d.",i);
        xprintf("%-16s",g_shell_cmd_list_ast[i].name);
        xprintf("%s\r\n",g_shell_cmd_list_ast[i].helpstr);
    }
}

static void spirxfunc(uint8_t* param)
{
	int id;
	int cs;
	int len;
	int flag;
	uint8_t* rx_tmp;
  if(4 == sscanf((const char*)param, "%*s %d %d %d %d", &id, &cs, &len, &flag))
  {
    xprintf("spitxrx %d %d %d %d\r\n",id,cs,len,flag);
		rx_tmp = pvPortMalloc(len);
		spi_trans(id,cs,0,rx_tmp,len,flag);
		xprintf("[rx]:");
		for(int i=0; i<len; i++){
			xprintf("%02x ", rx_tmp[i]);
		}
		xprintf("\r\n");
		vPortFree(rx_tmp);
  }
  else
  {
    xprintf("param err");
  }
}

static void spitxrxfunc(uint8_t* param)
{
	int id;
	int cs;
  uint8_t hexstr[32+1];
	uint8_t tx_tmp[16];
	uint8_t rx_tmp[16];
	uint32_t hexnum;
	int flag;
  if(4 == sscanf((const char*)param, "%*s %d %d %s %d", &id, &cs, hexstr, &flag))
  {
    xprintf("spitxrx %d %d %s %d\r\n",id,cs,hexstr,flag);
		hexnum = str2hex((const char*)hexstr,tx_tmp,32);
		spi_trans(id,cs,tx_tmp,rx_tmp,hexnum,flag);
		xprintf("[rx]:");
		for(uint32_t i=0; i<hexnum; i++){
			xprintf("%02x ", rx_tmp[i]);
		}
		xprintf("\r\n");
  }
  else
  {
    xprintf("param err");
  }
}

static void printflashfunc(uint8_t* param)
{
  uint8_t buffer[16];
  uint32_t addr;
  uint32_t len;
  if(2 == sscanf((const char*)param, "%*s %x %d", &addr, &len))
  {
    uint32_t toread;
    uint32_t read = 0;
    while(read < len)
    {
      toread = ((len-read) > sizeof(buffer)) ? sizeof(buffer) : (len-read);
			flash_itf_read(buffer, addr+read, toread);
      read += toread;
      for(uint32_t i=0; i<toread ;i++)
      {
        xprintf("%02x ",buffer[i]);
      }
      xprintf("\r\n");
    }
  }
}

static void writeflashfunc(uint8_t* param)
{
  uint8_t buffer[32+1];
  uint8_t hex[16];
	
  uint32_t addr;
  uint32_t len;
  if(2 == sscanf((const char*)param, "%*s %x %s", &addr, buffer))
  {
		len = str2hex((const char*)buffer, hex, strlen((char*)buffer));
		if(len>0)
    {
			flash_itf_write(hex,addr,len);
		}
  }
}

void printfilefunc(uint8_t* param)
{
  char path[128];
  uint8_t tmp[16];
  uint32_t addr;
  uint32_t size;
  FRESULT res;
  FIL fil;
  UINT br;
  uint32_t offset = 0;
  if(3 == sscanf((const char*)param, "%*s %s %x %d", path, &addr, &size))
  {
    xprintf("hexdump %s 0x%x %d\r\n",path,addr,size);
    if(FR_OK == (res=f_open(&fil,(const char*)path, FA_READ)))
    {
      xprintf("\r\n");
      do
      {
        br = 0;
        if(FR_OK == (res = f_read(&fil,tmp,(size>16)?16:size,&br)))
        {
          xprintf("%08x ",offset);
          offset+=br;
          for(uint32_t i=0;i<br;i++)
          {
            xprintf("%02x",(uint32_t)tmp[i]);
          }
          xprintf(":");
          for(uint32_t i=0;i<br;i++)
          {
            xprintf("%c",((tmp[i]>0x1F)&&(tmp[i]<0x7F))?(char)tmp[i]:'.');
          }
          xprintf("\r\n");
          size -= br;
        }
        else
        {
          break;
        }
      }while(br > 0);

      if(FR_OK != (res = f_close(&fil)))
      {
        xprintf("close %s err %d\r\n",path,res); 
      }
    }
    else
    {
      xprintf("open %s err %d",path,res);
    }
  }
  else
  {
    xprintf("param err");
  }
}

void writefilefunc(uint8_t* param)
{
  char path[128];
  uint8_t hexstr[32+1];
  uint8_t tmp[16];
  uint32_t hexnum = 0;
  uint32_t addr;
  FRESULT res;
  FIL fil;
  UINT bw;
  if(3 == sscanf((const char*)param, "%*s %s %x %s", path, &addr, hexstr))
  {
    xprintf("hexwrite %s 0x%x %s\r\n",path,addr,hexstr);
    if(FR_OK == (res=f_open(&fil,(const char*)path, FA_WRITE | FA_CREATE_ALWAYS)))
    {
      xprintf("\r\n");
      hexnum = str2hex((const char*)hexstr,tmp,32);
      if(hexnum > 0)
      {
        if(FR_OK == (res=(f_lseek(&fil,addr))))
        {
          if(FR_OK != (res = f_write(&fil,tmp,hexnum,&bw)))
          {
            xprintf("write err %d\r\n",res);
          }
        }
        else
        {
          xprintf("seek %d err %d\r\n",addr,res);
        }
      }

      if(FR_OK != (res = f_close(&fil)))
      {
        xprintf("close %s err %d\r\n",path,res); 
      }
    }
    else
    {
      xprintf("open %s err %d",path,res);
    }
  }
  else
  {
    xprintf("param err");
  }
}


