//////////////////////////////////////////////////////////////////////////////////	 
//本程序移植自网友ye781205的NES模拟器工程		  
////////////////////////////////////////////////////////////////////////////////// 	 
#include <string.h>
#include <stdint.h>
#include "nes_main.h" 
#include "nes_ppu.h"
#include "nes_mapper.h"
#include "nes_apu.h"

#include "FreeRTOS.h"
#include "xprintf.h"
#include "ff.h"
#include "key.h"

int MapperNo;			//map编号
int NES_scanline;		//nes扫描线
uint32_t VROM_1K_SIZE;
int VROM_8K_SIZE;

u8 PADdata;   			//手柄1键值 [7:0]右7 左6 下5 上4 Start3 Select2 B1 A0  
u8 PADdata1;   			//手柄2键值 [7:0]右7 左6 下5 上4 Start3 Select2 B1 A0  
u8 *NES_RAM;			//保持1024字节对齐
u8 *NES_SRAM;  
NES_header *RomHeader; 	//rom文件头
MAPPER *NES_Mapper;		 
MapperCommRes *MAPx;  


u8* spr_ram;			//精灵RAM,256字节
ppu_data* ppu;			//ppu指针
u8* VROM_banks;
u8* VROM_tiles;

//apu_t *apu; 			//apu指针


u8* romfile;							//nes文件指针,指向整个nes文件的起始地址.
//////////////////////////////////////////////////////////////////////////////////////

void* nes_malloc(uint32_t size)
{
	void* p = pvPortMalloc(size);
	if(p == (void*)0){
		xprintf("nes malloc %d err\r\n",size);
	}else{
		xprintf("nes malloc %d ok\r\n",size);
	}
	return p;
}

void nes_free(void* p)
{
	vPortFree(p);
	xprintf("nes free\r\n");
}


//加载ROM
//返回值:0,成功
//    1,内存错误
//    3,map错误
u8 nes_load_rom(void)
{  
    u8* p;  
	u8 i;
	u8 res=0;
	p=(u8*)romfile;	
	if(strncmp((char*)p,"NES",3)==0)
	{  
		RomHeader->ctrl_z=p[3];
		RomHeader->num_16k_rom_banks=p[4];
		RomHeader->num_8k_vrom_banks=p[5];
		RomHeader->flags_1=p[6];
		RomHeader->flags_2=p[7]; 
		if(RomHeader->flags_1&0x04)p+=512;		//有512字节的trainer:
		if(RomHeader->num_8k_vrom_banks>0)		//存在VROM,进行预解码
		{		
			VROM_banks=p+16+(RomHeader->num_16k_rom_banks*0x4000);
#if	NES_RAM_SPEED==1	//1:内存占用小 0:速度快	 
			VROM_tiles=VROM_banks;	 
#else  
			VROM_tiles=nes_malloc(RomHeader->num_8k_vrom_banks*8*1024);//这里可能申请多达1MB内存!!!
			if(VROM_tiles==0)VROM_tiles=VROM_banks;//内存不够用的情况下,尝试VROM_titles与VROM_banks共用内存			
			compile(RomHeader->num_8k_vrom_banks*8*1024/16,VROM_banks,VROM_tiles);  
#endif	
		}else 
		{
			VROM_banks=nes_malloc(8*1024);
			VROM_tiles=nes_malloc(8*1024);
			if(!VROM_banks||!VROM_tiles)res=1;
		}  	
		VROM_1K_SIZE = RomHeader->num_8k_vrom_banks * 8;
		VROM_8K_SIZE = RomHeader->num_8k_vrom_banks;  
		MapperNo=(RomHeader->flags_1>>4)|(RomHeader->flags_2&0xf0);
		if(RomHeader->flags_2 & 0x0E)MapperNo=RomHeader->flags_1>>4;//忽略高四位，如果头看起来很糟糕 
		xprintf("use map:%d\r\n",MapperNo);
		for(i=0;i<255;i++)  // 查找支持的Mapper号
		{		
			if (MapTab[i]==MapperNo)break;		
			if (MapTab[i]==-1)res=3; 
		} 
		if(res==0)
		{
			switch(MapperNo)
			{
				case 1:  
					MAP1=nes_malloc(sizeof(Mapper1Res)); 
					if(!MAP1)res=1;
					break;
				case 4:  
				case 6: 
				case 16:
				case 17:
				case 18:
				case 19:
				case 21: 
				case 23:
				case 24:
				case 25:
				case 64:
				case 65:
				case 67:
				case 69:
				case 85:
				case 189:
					MAPx=nes_malloc(sizeof(MapperCommRes)); 
					if(!MAPx)res=1;
					break;  
				default:
					break;
			}
		}
	} 
	return res;	//返回执行结果
} 
//释放内存 
void nes_sram_free(void)
{ 
	u8 i;
	nes_free(NES_RAM);	
	nes_free(NES_SRAM);	
	nes_free(RomHeader);	
	nes_free(NES_Mapper);
	nes_free(spr_ram);		
	nes_free(ppu);	
	//nes_free(apu);	
	if((VROM_tiles!=VROM_banks)&&VROM_banks&&VROM_tiles)//如果分别为VROM_banks和VROM_tiles申请了内存,则释放
	{
		nes_free(VROM_banks);
		nes_free(VROM_tiles);		 
	}
	switch (MapperNo)//释放map内存
	{
		case 1: 			//释放内存
			nes_free(MAP1);
			break;	 	
		case 4: 
		case 6: 
		case 16:
		case 17:
		case 18:
		case 19:
		case 21:
		case 23:
		case 24:
		case 25:
		case 64:
		case 65:
		case 67:
		case 69:
		case 85:
		case 189:
			nes_free(MAPx);break;	 		//释放内存 
		default:break; 
	}
	
	NES_SRAM=0;
	RomHeader=0;
	NES_Mapper=0;
	spr_ram=0;
	ppu=0;
	//apu=0;
	VROM_banks=0;
	VROM_tiles=0; 
	MAP1=0;
	MAPx=0;
} 
//为NES运行申请内存
//romsize:nes文件大小
//返回值:0,申请成功
//       1,申请失败
u8 nes_sram_malloc(void)
{
	xprintf("need mem:%d\r\n",0x800+0x2000+sizeof(NES_header)+sizeof(MAPPER)+0x100+sizeof(ppu_data));
		
	u16 i=0;
	//for(i=0;i<64;i++)//为NES_RAM,查找1024对齐的内存
	//{
	//	NES_SRAM=nes_malloc(i*32);
	NES_RAM=(uint8_t*)((uint32_t)nes_malloc(0X800+0x400) & 0xFFFFFC00);	//申请2K字节,必须1024字节对齐
	//	if((u32)NES_RAM%1024)			//不是1024字节对齐
	//	{
	//		nes_free(NES_RAM);		//释放内存,然后重新尝试分配
	//		nes_free(NES_SRAM); 
	//	}else 
	//	{
	//		nes_free(NES_SRAM); 	//释放内存
	//		break;
	//	}
	//}
	//if(i==64){
	//	xprintf("NES malloc i==64\n");
	//	return 1;
	//}
 	NES_SRAM=nes_malloc(0X2000);
	if(NES_SRAM==0){
	   xprintf("NES malloc NES_SRAM\n");
	}
	RomHeader=nes_malloc(sizeof(NES_header));
	if(RomHeader==0){
	   xprintf("NES malloc RomHeader\n");
	}
	NES_Mapper=nes_malloc(sizeof(MAPPER));
	if(NES_Mapper==0){
	   xprintf("NES malloc NES_Mapper\n");
	}
	spr_ram=nes_malloc(0X100);		
	if(spr_ram==0){
	   xprintf("NES malloc spr_ram\n");
	}
	ppu=nes_malloc(sizeof(ppu_data));  
	if(ppu==0){
	   xprintf("NES malloc ppu\n");
	}

	if(!NES_RAM||!NES_SRAM||!RomHeader||!NES_Mapper||!spr_ram||!ppu)
	{
	    xprintf("NES malloc failed!\n");
		nes_sram_free();
		return 1;
	}
	memset(NES_RAM,0,0X800);				//清零
	memset(NES_SRAM,0,0X2000);				//清零
	memset(RomHeader,0,sizeof(NES_header));	//清零
	memset(NES_Mapper,0,sizeof(MAPPER));	//清零
	memset(spr_ram,0,0X100);				//清零
	memset(ppu,0,sizeof(ppu_data));			//清零
	return 0;
} 

//开始nes游戏
//pname:nes游戏路径
//返回值:
//0,正常退出
//1,内存错误
//2,文件错误
//3,不支持的map
u8 nes_load(const char* pname)
{
	FIL *file; 
	UINT br;
	u8 res=0;   
	file=nes_malloc(sizeof(FIL));  
	if(file==0)return 1;						//内存申请失败.  
	res=f_open(file,(char*)pname,FA_READ);
	if(res!=FR_OK)	//打开文件失败
	{
		nes_free(file);
		return 2;
	}	 
	romfile=nes_malloc(f_size(file));			//申请游戏rom空间,等于nes文件大小 
 	if(romfile==0){
	   xprintf("NES malloc romfile\n");
	}
	memset(romfile,0,f_size(file));
	f_read(file,romfile,f_size(file),&br);	//读取nes文件
	f_close(file);
	nes_free(file);//释放内存
	return res;
} 

u8 nes_play_romfile(void)
{
	u8 res=0;   
	
	res=nes_sram_malloc();			//申请内存 
	if(res==0)
	{
		res=nes_load_rom();						//加载ROM
		if(res==0) 					
		{  
			Mapper_Init();						//map初始化
			cpu6502_init();						//初始化6502,并复位	  	 
			PPU_reset();						//ppu复位
			//apu_init(); 						//apu初始化 
			//nes_sound_open(0,APU_SAMPLE_RATE);	//初始化播放设备
			nes_emulate_frame();				//进入NES模拟器主循环 
			//nes_sound_close();					//关闭声音输出
		}
	}
	nes_sram_free();	//释放内存
	return res;
} 

u8 nes_xoff=0;	//显示在x轴方向的偏移量(实际显示宽度=256-2*nes_xoff)
//设置游戏显示窗口
void nes_set_window(void)
{	
	u16 xoff=0,yoff=0; 
	u16 lcdwidth,lcdheight;
	lcdwidth=240;
	lcdheight=240;
	nes_xoff=(256-240)/2;	//得到x轴方向的偏移量
	xoff=0; 
	yoff=(320-lcdheight)/2;//屏幕高度 
	//ILI9341_OpenWindow(xoff,yoff,lcdwidth,lcdheight);//让NES始终在屏幕的正中央显示
}
//extern void KEYBRD_FCPAD_Decode(uint8_t *fcbuf,uint8_t mode);
//读取游戏手柄数据
void nes_get_gamepadval(void)
{ 
	uint8_t key;
	uint8_t data = 0;
  /* key输入值  U D  L R    4        5      6  7     
	 *            3 1  2 0    SELECT   START  B  A
	 * 转为PADdata:[7:0]右7 左6 下5 上4 Start3 Select2 B1 A0	 
	 */
	key = key_get();
	if((key & ((uint8_t)1<<0)) == 0){
		data |= ((uint8_t)1<<7);
	}
	if((key & ((uint8_t)1<<1)) == 0){
		data |= ((uint8_t)1<<5);
	}
	if((key & ((uint8_t)1<<2)) == 0){
		data |= ((uint8_t)1<<6);
	}
	if((key & ((uint8_t)1<<3)) == 0){
		data |= ((uint8_t)1<<4);
	}	
	if((key & ((uint8_t)1<<4)) == 0){
		data |= ((uint8_t)1<<2);
	}
	if((key & ((uint8_t)1<<5)) == 0){
		data |= ((uint8_t)1<<3);
	}
	if((key & ((uint8_t)1<<6)) == 0){
		data |= ((uint8_t)1<<1);
	}
	if((key & ((uint8_t)1<<7)) == 0){
		data |= ((uint8_t)1<<0);
	}
	PADdata=data;
	PADdata1=0;				//没有手柄2,故不采用. 
}  

//nes模拟器主循环
void nes_emulate_frame(void)
{  
	uint8_t frame_cnt; 
	u8 nes_frame=0;
	nes_set_window();//设置窗口
	while(1)
	{	
		// LINES 0-239
		PPU_start_frame();
		for(NES_scanline = 0; NES_scanline< 240; NES_scanline++)
		{
			run6502(113*256);
			NES_Mapper->HSync(NES_scanline);
			//扫描一行		  
			if(nes_frame==0)scanline_draw(NES_scanline);
			else do_scanline_and_dont_draw(NES_scanline); 
		}  
		NES_scanline=240;
		run6502(113*256);//运行1线
		NES_Mapper->HSync(NES_scanline); 
		start_vblank(); 
		if(NMI_enabled()) 
		{
			cpunmi=1;
			run6502(7*256);//运行中断
		}
		NES_Mapper->VSync();
		// LINES 242-261    
		for(NES_scanline=241;NES_scanline<262;NES_scanline++)
		{
			run6502(113*256);	  
			NES_Mapper->HSync(NES_scanline);		  
		}	   
		end_vblank(); 
		nes_get_gamepadval();	//每3帧查询一次手柄输入
		//apu_soundoutput();		//输出游戏声音	 
		frame_cnt++; 	
		nes_frame++;
		if(nes_frame>NES_SKIP_FRAME)
		{
			nes_frame=0;//跳帧  
		}
		if(frame_cnt>=10)
		{
 			/*if(Key2_Scan())
			{
 				break;
			}*/
			frame_cnt=0;
 		}
	}
	//ILI9341_OpenWindow(0,0,240,320);//恢复屏幕窗口
}
//在6502.s里面被调用
void debug_6502(u16 reg0,u8 reg1)
{
	xprintf("6502 error:%x,%d\r\n",reg0,reg1);
}

void nes_apu_fill_buffer(int samples,u8* wavebuf)
{	 
	(void)samples;
	(void)wavebuf;
	
}	

void nes_main(char* name)
{
	nes_load(name);
	nes_play_romfile();
}