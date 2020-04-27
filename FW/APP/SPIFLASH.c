/*
 * SPIFLASH.c
 *
 *  Created on: Apr 27, 2020
 *      Author: Administrator
 */
#include "SPIFLASH.h"
#include "w25qxx.h"

static void spiflash_bin_read(uint8_t *buf,size_t offset,size_t length)
{
	//W25qxx_Init();
	W25qxx_ReadBytes(buf,offset,length);
}

static void spiflash_bin_write(uint8_t *buf,size_t offset,size_t length)
{
	//W25qxx_Init();
	//此处未进行擦除操作
	W25qxx_WritePage(buf,offset/w25qxx.PageSize,0,w25qxx.PageSize);
	W25qxx_WritePage(buf,offset/w25qxx.PageSize+1,0,w25qxx.PageSize);

}




static VirtualFat_File spiflash=
{
{'2','5','Q','X','X'},
{'B','I','N'},
spiflash_bin_read,
spiflash_bin_write,
8*1024*1024/8
};


void spiflash_bin_register(void)
{
	VirtualFat_Unregister_RootFile(&spiflash);
	if(!W25qxx_Init())
		return;//未连接spiflash
	if(w25qxx.CapacityInKiloByte!=0)
	{
		spiflash.size=w25qxx.CapacityInKiloByte*1024;
	}
	VirtualFat_Register_RootFile(&spiflash);

}
