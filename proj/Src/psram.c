#include "psram.h"
#include "py32f4xx_hal.h"
#include "xprintf.h"

ESMC_HandleTypeDef hesmc;
	
void psram_init(uint32_t baud)
{
	GPIO_InitTypeDef  GPIO_InitStruct;       
	__HAL_RCC_GPIOA_CLK_ENABLE(); 
	__HAL_RCC_GPIOB_CLK_ENABLE();  
	__HAL_RCC_ESMC_CLK_ENABLE();
		
	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;              
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;                    
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;    
	GPIO_InitStruct.Alternate = GPIO_AF10_ESMC; 
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);  

	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);  
	
	hesmc.Instance = ESMC;
	hesmc.Init.ClockMode = ESMC_CLOCK_MODE_0;
	uint32_t clk = HAL_RCC_GetHCLKFreq();
	uint32_t div = clk/baud;
	hesmc.Init.ClockPrescaler = div & 0xFE;

	hesmc.Init.DualFlash = ESMC_DUALFLASH_DISABLE;
	HAL_ESMC_Init(&hesmc);
	
}

void psram_set_rd(void)
{
	ESMC_CommandTypeDef cmd;
	cmd.Address = 0;
	cmd.AddressMode = ESMC_ADDRESS_SINGLE_LINE;
	cmd.AddressSize = ESMC_ADDRESS_24_BITS;
	cmd.AlternateByte = 0;
	cmd.AlternateByteMode = ESMC_ALTERNATE_BYTES_DISABLE;
	cmd.CSPinSel = ESMC_SELECT_PIN_CS0;
	cmd.DataMode = ESMC_DATA_READ;
	cmd.DdrMode = ESMC_DDR_DISABLE;
	cmd.DummyCycles = 1;
	cmd.Instruction = 0x03;
	cmd.InstructionMode = ESMC_INSTRUCTION_SINGLE_LINE;
	cmd.NbData = 4;
	cmd.TransferFormat = ESMC_TRANSFER_FORMAT_SINGLE;
	HAL_ESMC_MemoryMapped(&hesmc, &cmd);
}

void psram_set_wr(void)
{
	ESMC_CommandTypeDef cmd;
	cmd.Address = 0;
	cmd.AddressMode = ESMC_ADDRESS_SINGLE_LINE;
	cmd.AddressSize = ESMC_ADDRESS_24_BITS;
	cmd.AlternateByte = 0;
	cmd.AlternateByteMode = ESMC_ALTERNATE_BYTES_DISABLE;
	cmd.CSPinSel = ESMC_SELECT_PIN_CS0;
	cmd.DataMode = ESMC_DATA_READ;
	cmd.DdrMode = ESMC_DDR_DISABLE;
	cmd.DummyCycles = 1;
	cmd.Instruction = 0x02;
	cmd.InstructionMode = ESMC_INSTRUCTION_SINGLE_LINE;
	cmd.NbData = 4;
	cmd.TransferFormat = ESMC_TRANSFER_FORMAT_SINGLE;
	HAL_ESMC_MemoryMapped(&hesmc, &cmd);
}

void psram_read_id(uint8_t* buffer)
{
	ESMC_CommandTypeDef cmd;
	cmd.Address = 0;
	cmd.AddressMode = ESMC_ADDRESS_SINGLE_LINE;
	cmd.AddressSize = ESMC_ADDRESS_24_BITS;
	cmd.AlternateByte = 0;
	cmd.AlternateByteMode = ESMC_ALTERNATE_BYTES_DISABLE;
	cmd.CSPinSel = ESMC_SELECT_PIN_CS0;
	cmd.DataMode = ESMC_DATA_READ;
	cmd.DdrMode = ESMC_DDR_DISABLE;
	cmd.DummyCycles = 1;
	cmd.Instruction = 0x9F;
	cmd.InstructionMode = ESMC_INSTRUCTION_SINGLE_LINE;
	cmd.NbData = 8;
	cmd.TransferFormat = ESMC_TRANSFER_FORMAT_SINGLE;
	HAL_ESMC_Command(&hesmc, &cmd, 10);
	
	hesmc.RxXferSize = 8;
	HAL_ESMC_Receive(&hesmc, buffer, 10);
}

void psram_test(void)
{
	uint32_t* p = (uint32_t*)0x90000000;
	psram_set_wr();
	*p = 0x11223344;
	psram_set_rd();	
	xprintf("%x\r\n",*p);

}