#include <stdio.h>
#include "shell.h"
#include "shell_func.h"
#include "xprintf.h"
#include "spi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "flash_itf.h"
#include "ff.h"
#include "xmodem.h"
#include "ymodem.h"
#include "md5.h"
#include "uart.h"

static void helpfunc(uint8_t* param);
static void spirxfunc(uint8_t* param);
static void spitxrxfunc(uint8_t* param);
static void printflashfunc(uint8_t* param);
static void writeflashfunc(uint8_t* param);

static void printfilefunc(uint8_t* param);
static void writefilefunc(uint8_t* param);
static void rxfilefunc(uint8_t* param);
static void sxfilefunc(uint8_t* param);
static void ryfilefunc(uint8_t* param);
static void syfilefunc(uint8_t* param);
static void lsfilefunc(uint8_t* param);
static void rmfilefunc(uint8_t* param);
static void renamefilefunc(uint8_t* param);
static void touchfunc(uint8_t* param);
static void mkdirfunc(uint8_t* param);
static void pwdfunc(uint8_t* param);
static void cdfunc(uint8_t* param);
static void cpfilefunc(uint8_t* param);
static void md5sumfunc(uint8_t* param);
static void freefunc(uint8_t* param);
static void mkfsfunc(uint8_t* param);
static void mountfsfunc(uint8_t* param);
static void psfunc(uint8_t* param);
static void printmemfunc(uint8_t* param);
static void setmemfunc(uint8_t* param);
static void rxmemfunc(uint8_t* param);
static void sxmemfunc(uint8_t* param);

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
  { (uint8_t*)"rxfile",       rxfilefunc,       (uint8_t*)"rxfile name len"},
  { (uint8_t*)"sxfile",       sxfilefunc,       (uint8_t*)"sxfile name len"}, 
  { (uint8_t*)"ryfile",       ryfilefunc,       (uint8_t*)"ryfile"},
  { (uint8_t*)"syfile",       syfilefunc,       (uint8_t*)"syfile len path1 path2"},
  { (uint8_t*)"ls    ",       lsfilefunc,       (uint8_t*)"ls path"},
  { (uint8_t*)"rm    ",       rmfilefunc,       (uint8_t*)"rm path"},  
  { (uint8_t*)"rename    ",   renamefilefunc,   (uint8_t*)"rename path newpath"},  
  { (uint8_t*)"touch     ",   touchfunc,        (uint8_t*)"touch path size"},  
  { (uint8_t*)"mkdir     ",   mkdirfunc,        (uint8_t*)"mkdir path"},  
  { (uint8_t*)"pwd     ",     pwdfunc,          (uint8_t*)"pwd"},  
  { (uint8_t*)"cd     ",      cdfunc,           (uint8_t*)"cd path"},  
  { (uint8_t*)"cp",           cpfilefunc,       (uint8_t*)"cp srcpath dstpath"}, 
  { (uint8_t*)"md5sum",       md5sumfunc,       (uint8_t*)"md5sum path"}, 
  { (uint8_t*)"free",         freefunc,         (uint8_t*)"free path"},
  { (uint8_t*)"mkfs",         mkfsfunc,         (uint8_t*)"mkfs path"},
  { (uint8_t*)"mountfs",      mountfsfunc,      (uint8_t*)"mountfs path"},
	
  { (uint8_t*)"ps",    psfunc,    (uint8_t*)"ps"},	
	
  { (uint8_t*)"printmem",     printmemfunc,     (uint8_t*)"printmem mode[hex/dec] addr[hex] len datasize[8/16/32] sig[1/0]"}, 
  { (uint8_t*)"setmem",       setmemfunc,       (uint8_t*)"setmem addr[hex] val[hex]"}, 
  { (uint8_t*)"rxmem",        rxmemfunc,        (uint8_t*)"rxmem addr[hex] len"}, 
  { (uint8_t*)"sxmem",        sxmemfunc,        (uint8_t*)"sxmem addr[hex] len"}, 
	
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


static uint8_t rxtx_buf[1029];

static uint32_t getms(void)
{
    uint32_t ticks = xTaskGetTickCount();
    return ticks / (configTICK_RATE_HZ / 1000);
}

static uint32_t io_read(uint8_t* buffer, uint32_t len)
{
  return uart_read(0,buffer, len);
}

static void io_read_flush(void)
{
  uint8_t tmp;
  while(0 != uart_read(0,&tmp, 1));
}

static uint32_t io_write(uint8_t* buffer, uint32_t len)
{
  uart_send(0,buffer, len);
  return len;
}

static FIL rx_fil;            /* File object */
static int rx_file_open_flag = 0;


static int rx_file_open(char* name)
{
  FRESULT res = f_open(&rx_fil, name, FA_CREATE_NEW | FA_WRITE);
  if (FR_OK == res)
  {
    rx_file_open_flag = 1;
    return 0;
  }
  else
  {
    xprintf("open %s err %d\r\n",name,res);
    return -1;
  }
}

static int rx_file_close(char* name)
{
  (void)name;
  if(rx_file_open_flag != 0)
  {
    rx_file_open_flag = 0;
    FRESULT res = f_close(&rx_fil);
    if(res != FR_OK)
    {
      xprintf("close err %d\r\n",res); 
    }
    return 0;
  }
  else
  {
    return -1;
  }
}

static uint32_t  rx_file_write(uint32_t addr, uint8_t* buffer, uint32_t len)
{
  (void)addr;
  if(rx_file_open_flag != 0)
  {
    UINT bw;
    FRESULT res = f_write(&rx_fil, buffer, len, &bw);
    if ((bw != len) || (res != FR_OK))
    {
      xprintf("write err %d %d\r\n",bw,res);
    }
    return bw;
  }
  else
  {
    return 0;
  }
}

void rxfilefunc(uint8_t* param)
{
  char name[64];
  uint32_t len;
  int res = 0;
  if(2 == sscanf((const char*)param, "%*s %s %d", name, &len))
  {
    xprintf("rxfile %s %ld\r\n",name,len);
    if(0 == rx_file_open(name))
    {
      xmodem_cfg_st cfg=
      {
          .buffer = rxtx_buf,
          .crccheck = 1,
          .getms = getms,
          .io_read = io_read,
          .io_read_flush = io_read_flush,
          .io_write = io_write,
          .start_timeout = 60,
          .packet_timeout = 2000,
          .ack_timeout = 2000,
          .mem_write = rx_file_write,
          .addr = 0,
          .totallen = len,
      };
      xmodem_init_rx(&cfg);
      while((res = xmodem_rx()) == 0);
      rx_file_close(name);
      xprintf("res:%d\r\n",res);
    }
    else
    {
      xprintf("open:%s err\r\n",name);
    }
  }
}


static FIL tx_fil;            /* File object */
static int tx_file_open_flag = 0;

static int tx_file_open(char* name)
{
  FRESULT res = f_open(&tx_fil, name,  FA_READ);
  if (FR_OK == res)
  {
    tx_file_open_flag = 1;
    return 0;
  }
  else
  {
    xprintf("open %s err %d\r\n",name,res);
    return -1;
  }
}

static int tx_file_close(char* name)
{
  (void)name;
  if(tx_file_open_flag != 0)
  {
    tx_file_open_flag = 0;
    FRESULT res = f_close(&tx_fil);
    if(res != FR_OK)
    {
      xprintf("close err %d\r\n",res); 
    }
    return 0;
  }
  else
  {
    return -1;
  }
}

static uint32_t tx_file_read(uint32_t addr, uint8_t* buffer, uint32_t len)
{
  (void)addr;
  UINT br;
  FRESULT res = f_read(&tx_fil, buffer, len, &br);
  if(res != 0)
  {
    xprintf("read err %d\r\n",res); 
  }
  return br;
}

void sxfilefunc(uint8_t* param)
{
  char name[64];
  uint32_t len;
  int res = 0;
  if(2 == sscanf((const char*)param, "%*s %s %d", name, &len))
  {
    xprintf("sxfile %s %ld\r\n",name,len);
    if(0 == tx_file_open(name))
    {
      xmodem_cfg_st cfg=
      {
        .buffer = rxtx_buf,
        .plen = 1024,
        .getms = getms,
        .io_read = io_read,
        .io_read_flush = io_read_flush,
        .io_write = io_write,
        .start_timeout = 60,
        .packet_timeout = 1000,
        .ack_timeout = 5000,
        .mem_read = tx_file_read,
        .addr = 0,
        .totallen = len,
      };
      xmodem_init_tx(&cfg);
      while((res = xmodem_tx()) == 0);
      tx_file_close(name);
      xprintf("res:%d\r\n",res);
    }
    else
    {
      xprintf("open:%s err\r\n",name);
    }
  }
}

/* List contents of a directory */
static FRESULT list_dir (const char *path)
{
    FRESULT res;
    DIR dir;
    FILINFO fno;
    int nfile, ndir;


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        nfile = ndir = 0;
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Error or end of dir */
            if (fno.fattrib & AM_DIR) {            /* Directory */
                xprintf("   <DIR>   %s\n", fno.fname);
                ndir++;
            } else {                               /* File */
                xprintf("%10u %s\n", fno.fsize, fno.fname);
                nfile++;
            }
        }
        f_closedir(&dir);
        xprintf("%d dirs, %d files.\n", ndir, nfile);
    } else {
        xprintf("Failed to open \"%s\". (%u)\n", path, res);
    }
    return res;
}


void lsfilefunc(uint8_t* param)
{
  char path[128];
  if(1 == sscanf((const char*)param, "%*s %s", path))
  {
    list_dir((const char *)path);
  }
  else
  {
    xprintf("param err");
  }
}

void rmfilefunc(uint8_t* param)
{
  FRESULT res;
  char path[128];
  if(1 == sscanf((const char*)param, "%*s %s", path))
  {
    if(FR_OK != (res = f_unlink((const char *)path)))
    {
      xprintf("unlink %s err %d\r\n",path,res);
    }
  }
  else
  {
    xprintf("param err");
  }
}

void renamefilefunc(uint8_t* param)
{
  FRESULT res;
  char path[128];
  char newpath[128];
  if(2 == sscanf((const char*)param, "%*s %s %s", path, newpath))
  {
    if(FR_OK != (res = f_rename((const char *)path, (const char*)newpath)))
    {
      xprintf("rename %s to %s err %d\r\n",path,newpath,res);
    }
  }
  else
  {
    xprintf("param err");
  }
}

void touchfunc(uint8_t* param)
{
  FIL fil;
  char path[128];
  int size;
  if(2 == sscanf((const char*)param, "%*s %s %d", path, &size))
  {
    FRESULT res = f_open(&fil, path, FA_CREATE_NEW | FA_WRITE);
    if (FR_OK == res)
    {
      if(FR_OK != (res = f_expand(&fil,size,1)))
      {
        xprintf("expand %s size to %d err %d\r\n",path,size,res); 
      }

      if(FR_OK != (res = f_close(&fil)))
      {
        xprintf("close %s err %d\r\n",path,res); 
      }
    }
    else
    {
      xprintf("open %s err %d\r\n",path,res);
    }
  }
  else
  {
    xprintf("param err");
  }
}


void mkdirfunc(uint8_t* param)
{
  char path[128];
  FRESULT res;
  if(1 == sscanf((const char*)param, "%*s %s", path))
  {
    if(FR_OK != (res=f_mkdir((const char*)path)))
    {
      xprintf("mkdir %s err %d",path,res);
    }
  }
  else
  {
    xprintf("param err");
  }
}

void pwdfunc(uint8_t* param)
{
  (void)param;
  FRESULT fr;
  TCHAR str[128];
  if(1 == sscanf((const char*)param, "%*s %s", str)){
		if(FR_OK != (fr = f_getcwd(str, 128)))  /* Get current directory path */
		{
			xprintf("getcwd err %d\r\n",fr);
		}
		else
		{
			xprintf("%s\r\n",str);
		}
	}else
  {
    xprintf("param err");
  }
}


void cdfunc(uint8_t* param)
{
  char path[128];
  FRESULT res;
  if(1 == sscanf((const char*)param, "%*s %s", path))
  {
    if(FR_OK != (res=f_chdir((const char*)path)))
    {
      xprintf("chdir to %s err %d",path,res);
    }
  }
  else
  {
    xprintf("param err");
  }
}

/* md5算法参考 https://www.rfc-editor.org/rfc/rfc1321 */

void md5sumfunc(uint8_t* param)
{
  char path[128];
  uint8_t tmp[64];
  uint8_t out[16];
  FIL fil;
  UINT br;
  FRESULT res;

  MD5_CTX ctx;
  MD5Init(&ctx);
  if(1 == sscanf((const char*)param, "%*s %s",path))
  {
    res = f_open(&fil, path, FA_READ);
    if(res == FR_OK)
    {
      do
      {
        res = f_read(&fil, tmp, sizeof(tmp), &br);
        if(res == 0)
        {
          MD5Update(&ctx, tmp, br);
        }
        else
        {
          xprintf("read err %d\r\n",res); 
          break;
        }
      }while(br > 0);

      MD5Final(out, &ctx);
      for(int i=0; i<16;i++)
      { 
        xprintf("%02x",out[i]);
      }
      xprintf("\r\n");
      f_close(&fil);
    }
    else
    {
      xprintf("open %s err %d\r\n",path,res);
    }
  }
  else
  {
    xprintf("param err");
  }
}

void freefunc(uint8_t* param)
{
  char path[128];
  FRESULT res;
  if(1 == sscanf((const char*)param, "%*s %s", path))
  {
    FATFS *fs;
    DWORD fre_clust, fre_sect, tot_sect;
    /* Get volume information and free clusters of drive 1 */
    res = f_getfree(path, &fre_clust, &fs);
    if(res == FR_OK)
    {
      /* Get total sectors and free sectors */
      tot_sect = (fs->n_fatent - 2) * fs->csize;
      fre_sect = fre_clust * fs->csize;

      /* Print the free space (assuming 512 bytes/sector) */
      xprintf("%10lu KiB total drive space.\n%10lu KiB available.\n", tot_sect / 2, fre_sect / 2);
    }
    else
    {
      xprintf("getfree %s err %d\r\n",path,res);
    }
  }
  else
  {
    xprintf("param err");
  }
}

extern void app_fatfs_mkfs(char* path);
void mkfsfunc(uint8_t* param)
{
  char path[128];
  if(1 == sscanf((const char*)param, "%*s %s", path))
  {
		app_fatfs_mkfs(path);
  }
  else
  {
    xprintf("param err");
  }
}

extern void app_fatfs_mount(char* path);
void mountfsfunc(uint8_t* param)
{
  char path[128];
  if(1 == sscanf((const char*)param, "%*s %s", path))
  {
		app_fatfs_mount(path);
  }
  else
  {
    xprintf("param err");
  }
}

static FIL ymodem_rx_fil;            /* File object */
static int ymodem_rx_file_open_flag = 0;

static int  ymodem_rx_file_start(uint32_t addr, uint8_t** name, uint32_t* len)
{
  (void)addr;
  (void)len;
  int res = 0;
  if(ymodem_rx_file_open_flag == 0)
  {

    FRESULT res = f_open(&ymodem_rx_fil, (const char*)(*name), FA_CREATE_NEW | FA_WRITE);
    if (FR_OK == res)
    {
      ymodem_rx_file_open_flag = 1;
      return 0;
    }
    else
    {
      xprintf("open %s err %d\r\n",name,res);
      return -1;
    }
  }

  return res;
}

static uint32_t  ymodem_rx_file_write(uint32_t addr, uint8_t* buffer, uint32_t len)
{
  (void)addr;
  if(ymodem_rx_file_open_flag != 0)
  {
    UINT bw;
    FRESULT res = f_write(&ymodem_rx_fil, buffer, len, &bw);
    if ((bw != len) || (res != FR_OK))
    {
      xprintf("write err %d %d\r\n",bw,res);
    }
    return bw;
  }
  else
  {
    return 0;
  }
}

static int  ymodem_rx_file_done(void)
{
  if(0 != ymodem_rx_file_open_flag)
  {
    ymodem_rx_file_open_flag = 0;
    return f_close(&ymodem_rx_fil);
  }
  return 0;
}

void ryfilefunc(uint8_t* param)
{
  (void)param;
  int res = 0;

  ymodem_rx_cfg_st cfg=
  {
    .buffer = rxtx_buf,
    .getms = getms,
    .io_read = io_read,
    .io_write = io_write,
    .start_timeout = 60,
    .packet_timeout = 2000,
    .ack_timeout = 1000,
    .mem_start = ymodem_rx_file_start,
    .mem_write = ymodem_rx_file_write,
    .mem_done = ymodem_rx_file_done,
  };

  ymodem_rx_init(&cfg);
  while((res = ymodem_rx()) == YMODEM_RX_ERR_NEED_CONTINUE);

  xprintf("\r\nres:%d\r\n",res);
}


static int8_t ymodem_sfile_name[2][64];
static int ymodem_tx_file_open_flag = 0;
static uint32_t ymodem_tx_file_num = 0;
static FIL ymodem_tx_file;
static uint8_t ymodem_sfile_name_len_att[128];
/* 获取文件名 */
static int  ymodem_tx_file_start(uint32_t addr, uint8_t** name, uint32_t* len)
{
  (void)addr;
  (void)len;
  uint32_t flen;
  uint32_t fnamelen;
  int res = 0;
  if(ymodem_tx_file_open_flag != 0)
  { 
    f_close(&ymodem_tx_file);
    ymodem_tx_file_open_flag = 0;
  }
  if(ymodem_tx_file_num >= sizeof(ymodem_sfile_name)/sizeof(ymodem_sfile_name[0]))
  {
    return -1;
  }


  if(0 == (res = f_open(&ymodem_tx_file, (const char*)ymodem_sfile_name[ymodem_tx_file_num], FA_READ)))
  {
    flen =  f_size(&ymodem_tx_file);
    fnamelen = strlen((const char*)ymodem_sfile_name[ymodem_tx_file_num]);
    memcpy(ymodem_sfile_name_len_att,ymodem_sfile_name[ymodem_tx_file_num],fnamelen);
    ymodem_sfile_name_len_att[fnamelen]=0;
    *len = fnamelen + 1 + snprintf((char*)(&ymodem_sfile_name_len_att[fnamelen+1]),sizeof(ymodem_sfile_name_len_att)-(fnamelen+1),"%d %o %d",flen,1715670058,0);
    *name = ymodem_sfile_name_len_att;
    ymodem_tx_file_open_flag = 1;

  }
  ymodem_tx_file_num++;
  return res;
}

static uint32_t ymodem_tx_file_read(uint32_t addr, uint8_t* buffer, uint32_t len)
{
  (void)addr;
  UINT br;
  FRESULT res = f_read(&ymodem_tx_file, buffer, len, &br);
  if(res != 0)
  {
    xprintf("read err %d\r\n",res); 
  }
  return br;
}


static int  ymodem_tx_file_done(void)
{
  if(0 != ymodem_tx_file_open_flag)
  {
    ymodem_tx_file_open_flag = 0;
    return f_close(&ymodem_tx_file);
  }
  return 0;
}

void syfilefunc(uint8_t* param)
{
  (void)param;
  int plen = 0;
  int res;
  int num = 0;
  ymodem_tx_cfg_st cfg=
  {
    .buffer = rxtx_buf,
    .getms = getms,
    .io_read = io_read,
    .io_write = io_write,
    .start_timeout = 60,
    .packet_timeout = 2000,
    .ack_timeout = 2000,
    .plen = 1024,
    .mem_start = ymodem_tx_file_start,
    .mem_read = ymodem_tx_file_read,
    .mem_done = ymodem_tx_file_done,
  };

  memset(ymodem_sfile_name,0,sizeof(ymodem_sfile_name));
  ymodem_tx_file_num = 0;
  num = sscanf((const char*)param, "%*s %d %s %s", &plen, ymodem_sfile_name[0],ymodem_sfile_name[1]);
  if((num == 2) || (num == 3))
  {
    cfg.plen = plen;
    ymodem_tx_init(&cfg);
    while((res = ymodem_tx()) == YMODEM_TX_ERR_NEED_CONTINUE);

    xprintf("\r\nres:%d\r\n",res);
  }
}

static int cp(const char* src, const char* dst)
{
	FIL fr;
	FIL fw;
	BYTE buf[64] = {0x00};
	FRESULT res;    
	size_t write_size;  /*剩余待写入字节数*/    
	DWORD pos = 0;
	UINT btr;        /*一次试图写入字节数*/
	UINT br;         /*一次实际读到字节数*/
	UINT bw;         /*一次实际写入字节数*/
    UINT size;
	res = f_open(&fr, src, FA_READ);
	if (res != FR_OK)
	{
		return -1;
	}
	res = f_open(&fw, dst, FA_WRITE | FA_CREATE_ALWAYS);
	if (res != FR_OK) 
	{
		f_close(&fr);
		return -1;
	}    
	write_size = f_size( &fr ); 
    size = write_size; 
	do 
	{   
		btr = (write_size > sizeof(buf)) ? sizeof(buf) : write_size;
        //res = f_lseek(&fr, pos);
		res = f_read (&fr, buf, btr, &br); 
        if(res != FR_OK)
        {
            f_close(&fw);
            f_close(&fr);  
            return -1;
        }
        //res = f_lseek(&fw, pos);		
		res = f_write(&fw, buf, br, &bw);
        if((res != FR_OK) || (br != bw))
        {
            f_close(&fw);
            f_close(&fr); 
            return -1;
        }
		//f_sync(&fw);
		pos += br;
		write_size -= br;
        xprintf("\b\b%02d", 100 * (pos + 1) / size);
	}while(write_size > 0);
    xprintf("\b\b\b%d\r\n", 100);
	res = f_close(&fw);
	res = f_close(&fr);  
    return 0;
}

static void cpfilefunc(uint8_t * cmdbuf)
{
	int len;
	int res;
	char path[32] = {0x00};
	char dst[32] = {0x00};
	char regexp[32]; 
	memset((void *)path, 0, sizeof(path));
  len = sscanf((char const *)cmdbuf, "%*s %31s %31s %31s", path,dst,regexp);
	if (len == 2)
	{
		xprintf("\tcopy \"%s\" to \"%s\"...\r\n", path, dst);
		res = cp(path, dst);
		xprintf("f_cp result = %d\r\n", res);
	}
  else
  {}
}

static void psfunc(uint8_t* param)
{
  (void)param;
  uint32_t tasknum = uxTaskGetNumberOfTasks();
  if(tasknum > 0)
  {
    TaskStatus_t * p_task_array = pvPortMalloc(sizeof(TaskStatus_t) * tasknum);
    if (NULL == p_task_array) {
        return;
    }
		uint32_t *total_time=0;
    tasknum = uxTaskGetSystemState(p_task_array, tasknum, total_time);

		xprintf("name            id\tpri\tts\tsta\tsbase\t   smark\r\n");
    for (uint32_t i = 0; i < tasknum; i++) {
        TaskStatus_t * p_task = p_task_array + i;

        xprintf("%-16s%-8d%-8d%-8d%-8d%-8x%8d\r\n",p_task->pcTaskName,
			  p_task->xTaskNumber,
			  p_task->uxCurrentPriority,
			  p_task->ulRunTimeCounter,
        p_task->eCurrentState,
			 (uint32_t)(p_task->pxStackBase),
			  p_task->usStackHighWaterMark);
    }

    vPortFree(p_task_array);
  }
}


static void setmemfunc(uint8_t* param)
{
  uint32_t addr;
  uint32_t val;
  if(2 == sscanf((const char*)param, "%*s %x %x", &addr, &val))
  {
    xprintf("setmem  %x %x\r\n",addr,val);
    if((addr % 4) ==0)
    {
      *(volatile uint32_t*)addr = val;
      xprintf("%x\r\n",*(volatile uint32_t*)addr);
    }
    else
    {
      xprintf("addr must be mul of 4\r\n");
    }
  }
}

static void printmemfunc(uint8_t* param)
{
  uint32_t addr;
  uint32_t len;
  uint8_t mode[64];
  int datasize;
  uint8_t* tmp8_u;
  uint16_t* tmp16_u;
  uint32_t* tmp32_u;
  int8_t* tmp8_i;
  int16_t* tmp16_i;
  int32_t* tmp32_i;

  int sig;
  if(5 == sscanf((const char*)param, "%*s %s %x %d %d %d", mode, &addr, &len, &datasize, &sig))
  {
    if(strncmp((const char*)mode,"hex", 3) == 0)
    {
      if(datasize == 8)
      {
        tmp8_u = (uint8_t*)addr;
        for(uint32_t i=0; i<len ;i++)
        {
          if(i%16 == 0)
          {
            xprintf("\r\n[%08x]:",addr+i*1);
          }
          xprintf("%02x ",tmp8_u[i]);
        }
        xprintf("\r\n");
      }
      else if(datasize == 16)
      {
        tmp16_u = (uint16_t*)addr;
        for(uint32_t i=0; i<len ;i++)
        {
          if(i%16 == 0)
          {
            xprintf("\r\n[%08x]:",addr+i*2);
          }
          xprintf("%04x ",tmp16_u[i]);
        }
        xprintf("\r\n");
      }
      else if(datasize == 32)
      {
        tmp32_u = (uint32_t*)addr;
        for(uint32_t i=0; i<len ;i++)
        {
          if(i%16 == 0)
          {
            xprintf("\r\n[%08x]:",addr+i*4);
          }
          xprintf("%08x ",tmp32_u[i]);
        }
        xprintf("\r\n");
      }
      else
      {
        xprintf("datasize must be 8/16/32\r\n");
      }
    }
    else if(strncmp((const char*)mode,"dec", 3) == 0)
    {
      if(datasize == 8)
      {
        if(sig == 0)
        {
          tmp8_u = (uint8_t*)addr;
          for(uint32_t i=0; i<len ;i++)
          {
            if(i%16 == 0)
            {
              xprintf("\r\n");
            }
            xprintf("%d ",tmp8_u[i]);
          }
          xprintf("\r\n");
        }
        else
        {
          tmp8_i = (int8_t*)addr;
          for(uint32_t i=0; i<len ;i++)
          {
            if(i%16 == 0)
            {
              xprintf("\r\n");
            }
            xprintf("%d ",tmp8_i[i]);
          }
          xprintf("\r\n");
        }
      }
      else if(datasize == 16)
      {
        if(sig == 0)
        {
          tmp16_u = (uint16_t*)addr;
          for(uint32_t i=0; i<len ;i++)
          {
            if(i%16 == 0)
            {
              xprintf("\r\n");
            }
            xprintf("%d ",tmp16_u[i]);
          }
          xprintf("\r\n");
        }
        else
        {
          tmp16_i = (int16_t*)addr;
          for(uint32_t i=0; i<len ;i++)
          {
            if(i%16 == 0)
            {
              xprintf("\r\n");
            }
            xprintf("%d ",tmp16_i[i]);
          }
          xprintf("\r\n");
        }
      }
      else if(datasize == 32)
      {
        if(sig == 0)
        {
          tmp32_u = (uint32_t*)addr;
          for(uint32_t i=0; i<len ;i++)
          {
            if(i%16 == 0)
            {
              xprintf("\r\n");
            }
            xprintf("%d ",tmp32_u[i]);
          }
          xprintf("\r\n");
        }
        else
        {
          tmp32_i = (int32_t*)addr;
          for(uint32_t i=0; i<len ;i++)
          {
            if(i%16 == 0)
            {
              xprintf("\r\n");
            }
            xprintf("%d ",tmp32_i[i]);
          }
          xprintf("\r\n");
        }
      }
      else
      {
        xprintf("datasize must be 8/16/32\r\n");
      }
    }
    else
    {
      xprintf("mode must be [hex/dec]\r\n");
    }
  }
}

static uint32_t mem_read(uint32_t addr, uint8_t* buffer, uint32_t len)
{
  memcpy(buffer, (uint8_t*)addr, len);
  return len;
}

static uint32_t mem_write(uint32_t addr, uint8_t* buffer, uint32_t len)
{
  memcpy((uint8_t*)addr, buffer, len);
  return len;
}

static void rxmemfunc(uint8_t* param)
{
  uint32_t addr;
  uint32_t len;
  int res = 0;
  if(2 == sscanf((const char*)param, "%*s %x %d", &addr, &len))
  {
    xprintf("rxmem to 0x%x %d\r\n",addr,len);
    xmodem_cfg_st cfg=
      {
        .buffer = rxtx_buf,
        .crccheck = 1,
        .getms = getms,
        .io_read = io_read,
        .io_read_flush = io_read_flush,
        .io_write = io_write,
        .start_timeout = 60,
        .packet_timeout = 1000,
        .ack_timeout = 1000,
        .mem_write = mem_write,
        .addr = addr,
        .totallen = len,
      };
      xmodem_init_rx(&cfg);
      while((res = xmodem_rx()) == 0);
      xprintf("res:%d\r\n",res);
  }
}

static void sxmemfunc(uint8_t* param)
{
  uint32_t addr;
  uint32_t len;
  int res = 0;
  if(2 == sscanf((const char*)param, "%*s %x %d", &addr, &len))
  {
    xprintf("sxmem to 0x%x %d\r\n",addr,len);
    xmodem_cfg_st cfg=
    {
      .buffer = rxtx_buf,
      .plen = 1024,
      .getms = getms,
      .io_read = io_read,
      .io_read_flush = io_read_flush,
      .io_write = io_write,
      .start_timeout = 60,
      .packet_timeout = 1000,
      .ack_timeout = 5000,
      .mem_read = mem_read,
      .addr = addr,
      .totallen = len,
    };
    xmodem_init_tx(&cfg);
    while((res = xmodem_tx()) == 0);
    xprintf("res:%d\r\n",res);
  }
}
