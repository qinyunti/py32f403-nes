#ifndef PSRAM_H
#define PSRAM_H

#ifdef __cplusplus
extern "C" {
#endif
  
#include <stdint.h>

void psram_init(uint32_t baud);
void psram_read_id(uint8_t* buffer);
void psram_test(void);
#ifdef __cplusplus
}
#endif

#endif


