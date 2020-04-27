/*
 * EEPROM.c
 *
 *  Created on: Apr 27, 2020
 *      Author: Administrator
 */
#include "EEPROM.h"
#include "eeprom24xx.h"
#include "main.h"

static void eeprom_bin_read(uint8_t *buf,size_t offset,size_t length)
{
	EEPROM24XX_Load(offset,buf,length);
}

static void eeprom_bin_write(uint8_t *buf,size_t offset,size_t length)
{
	EEPROM24XX_Save(offset,buf,length);
}




static VirtualFat_File eeprom=
{
{'2','4','X','6','4'},
{'B','I','N'},
eeprom_bin_read,
eeprom_bin_write,
64*1024/8
};

extern size_t _EEPROM_SIZE_KBIT;//容量设置

void eeprom_bin_setsize(uint32_t size)
{
	if(size==0)
	{
		_EEPROM_SIZE_KBIT=64;
		return;
	}


	uint16_t size_1=0x01;
	for(size_t i=0;i<16;i++)
	{
		if(size_1&size)
		{
			_EEPROM_SIZE_KBIT=size_1;
		}
		size_1 <<=1;
	}



	{//写后备寄存器
			RCC->APB1ENR |= (1 << 27 | 1 << 28); 	// 电源接口时钟/备份时钟开启
			PWR->CR |= 1 << 8;	// 允许写入后备寄存器

			BKP->DR1=_EEPROM_SIZE_KBIT;
	}

	eeprom_bin_register();

}

void eeprom_bin_register(void)
{

	{
		RCC->APB1ENR |= (1 << 27 | 1 << 28); 	// 电源接口时钟/备份时钟开启
		PWR->CR |= 1 << 8;	// 允许写入后备寄存器

		if(BKP->DR1!=0)//使用BKP->DR1保存EEPROM大小
		{
			_EEPROM_SIZE_KBIT=BKP->DR1;
		}
	}

	VirtualFat_Unregister_RootFile(&eeprom);
	if(!EEPROM24XX_IsConnected()) return;//未连接eeprom
	sprintf(eeprom.FileName,"24X%02d",_EEPROM_SIZE_KBIT);
	eeprom.size=_EEPROM_SIZE_KBIT*1024/8;
	VirtualFat_Register_RootFile(&eeprom);

}
