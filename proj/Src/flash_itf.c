#include "flash_itf.h"
#include "flash.h"
#include "spi.h"
#include "FreeRTOS.h"

static flash_dev_st s_flash;

static uint32_t spi_io(uint8_t* tx_buffer, uint8_t* rx_buffer, uint32_t len, int flag)
{
	return spi_trans(0, 0, tx_buffer, rx_buffer, len, flag);
}


int flash_itf_init(void)
{
	s_flash.spi_trans = spi_io;
	s_flash.page_bits = 8;
	s_flash.page_size = 256;
	s_flash.sector_bits = 12;
	s_flash.sector_size = 4096;
	s_flash.buffer = pvPortMalloc(4096);
	return 0;
}

int flash_itf_deinit(void)
{
	vPortFree(s_flash.buffer);
	return 0;
}


uint32_t flash_itf_read(uint8_t* buffer, uint32_t addr, uint32_t len)
{
	return flash_read(&s_flash, buffer, addr, len);
}


uint32_t flash_itf_write(uint8_t* buffer, uint32_t addr, uint32_t len)
{
	return flash_write(&s_flash, buffer, addr, len);
}