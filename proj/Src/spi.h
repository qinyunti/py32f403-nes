#ifndef SPI_H
#define SPI_H

#ifdef __cplusplus
extern "C" {
#endif
  
#include <stdint.h>

void spi_init(int id, uint32_t baud, int mode);
uint32_t spi_trans(int id, int cs, uint8_t* tx_buffer, uint8_t* rx_buffer, uint32_t len, int flag);

#ifdef __cplusplus
}
#endif

#endif


