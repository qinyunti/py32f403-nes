//////////////////////////////////////////////////////////////////////////////////	 
//本程序移植自网友ye781205的NES模拟器工程		  
////////////////////////////////////////////////////////////////////////////////// 	 
#include <string.h>
#include "nes_apu.h"



/* Read from $4000-$4017 */
u8 Apu_Read4015(u32 address)//***********************************************************************
{
   u8 value=0;
	 return value;
}
void Apu_Write4015(u8 value,u32 address )
{
	(void)value;
	(void)address;
}

void Apu_Write(u8 value,u32 address )
{
	(void)value;
	(void)address;
} 
void apu_getpcmdata(void **data, int *num_samples, int *sample_bits)
{
}