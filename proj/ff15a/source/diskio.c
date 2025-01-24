#include "ff.h"			
#include "diskio.h"		
#include "flash_itf.h"
#include "xprintf.h"

#define DEV_FLASH		0	


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	switch (pdrv) {
	case DEV_FLASH :
		return RES_OK;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	switch (pdrv) {
	case DEV_FLASH :
		return RES_OK;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	switch (pdrv) {
	case DEV_FLASH :
		flash_itf_read(buff, sector*512, count*512);
	  //xprintf("read sec:%d,c:%d\r\n",sector,count);
		return RES_OK;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	switch (pdrv) {
	case DEV_FLASH :
		flash_itf_write((uint8_t*)buff, sector*512, count*512);
	  //xprintf("write sec:%d,c:%d\r\n",sector,count);
		return RES_OK;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	switch (pdrv) {
	case DEV_FLASH :

		switch(cmd)
		{
			case CTRL_SYNC:
				res = RES_OK;
			break;
			case GET_SECTOR_SIZE:
				*(WORD*)buff = 512;
				res = RES_OK;
			break;
			case GET_BLOCK_SIZE:
				*(WORD*)buff = 4096;
				res = RES_OK;
			break;
			case GET_SECTOR_COUNT:
				*(DWORD*)buff = 131072;
				res = RES_OK;
			break;
			case CTRL_TRIM:
				res = RES_OK;
			break;
			default:
				res = RES_PARERR;
			break;
		}
		return res;
	}

	return RES_PARERR;
}

