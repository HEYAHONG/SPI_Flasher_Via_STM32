/*
 * EEPROM.c
 *
 *  Created on: Apr 27, 2020
 *      Author: Administrator
 */
#include "EEPROM.h"
#include "eeprom24xx.h"

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

void eeprom_bin_register(void)
{
	VirtualFat_Unregister_RootFile(&eeprom);
	if(!EEPROM24XX_IsConnected()) return;//未连接eeprom
	sprintf(eeprom.FileName,"24X%d",_EEPROM_SIZE_KBIT);
	eeprom.size=_EEPROM_SIZE_KBIT*1024/8;
	VirtualFat_Register_RootFile(&eeprom);

}
