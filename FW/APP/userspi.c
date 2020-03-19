/*
 * userspi.c
 *
 *  Created on: Mar 18, 2020
 *      Author: Administrator
 */
#include "w25qxx.h"
#include "userapp.h"

//uint8数组转uint32函数,小端模式
static uint32_t U8A2U32(void * p)
{
	uint8_t * data=(uint8_t *)p;
	return (((uint32_t)data[3])<<24)+(((uint32_t)data[2])<<16)+(((uint32_t)data[1])<<8)+(((uint32_t)data[0])<<0);
}

static uint8_t spi_rx_buff[1000];

//初始化命令模式数据结构
spi_cmd_data_t spi_cmd_data=
{
.IsUpdate=0,
.data=spi_rx_buff,
.length=0
};

void delay(size_t delay)//中断中不能调用HAL_Delay时使用
{
while(delay--);
}
void * spi_transmitReceive(void * txbuff,size_t len)
{
	 SPI_CS_HIGH();
	 delay(48*5);//
	 SPI_CS_LOW();
	 delay(48*5);
	 memset(spi_rx_buff,0,sizeof(spi_rx_buff));
	 HAL_SPI_TransmitReceive(&hspi1,txbuff,spi_rx_buff,len,1000+len);
	 SPI_CS_HIGH();
	 return spi_rx_buff;
}

void spi_cmd_process_call()//spi命令模式处理函数
{
	if(spi_cmd_data.IsUpdate)//有新的命令
	{

		switch(spi_cmd_data.data[0])//判断命令号
		{
		default:
			spi_cmd_data.data[0]=0xff;//无效命令的返回
			spi_cmd_data.length=0x01;
			break;
		case 0://0号命令，设置spi通信参数，字节1为CPOL,字节2为CPHA,字节3字节序(MSB/LSB))
			if(spi_cmd_data.length>=2)
				hspi1.Init.CLKPolarity=spi_cmd_data.data[1]?SPI_POLARITY_HIGH:SPI_POLARITY_LOW;
			if(spi_cmd_data.length>=3)
				hspi1.Init.CLKPhase=spi_cmd_data.data[2]?SPI_PHASE_2EDGE:SPI_PHASE_1EDGE;
			if(spi_cmd_data.length>=4)
				hspi1.Init.FirstBit=spi_cmd_data.data[3]?SPI_FIRSTBIT_LSB:SPI_FIRSTBIT_MSB;
			HAL_SPI_Init(&hspi1);

			break;
		case 1://1号命令，重新初始化spi flash通信
			if(W25qxx_Init())
			{
				memcpy(&(spi_cmd_data.data[1]),&w25qxx,sizeof(w25qxx));
				spi_cmd_data.length=sizeof(w25qxx)+1;
			}
			else
			{
				spi_cmd_data.data[0]=0xff;//失败命令的返回
				spi_cmd_data.length=0x01;
			}

			break;

		case 2://2号命令，W25qxx_EraseChip,芯片擦除
			W25qxx_EraseChip();

			break;
		case 3://3号命令,W25qxx_EraseSector,擦除扇区，前四字节为扇区地址（小端模式）
			if(spi_cmd_data.length>=5)
				W25qxx_EraseSector(U8A2U32(&(spi_cmd_data.data[1])));
			else
				{
						spi_cmd_data.data[0]=0xff;//失败命令的返回
						spi_cmd_data.length=0x01;

				}

			break;
		case 4://4号命令，W25qxx_EraseBlock，擦除块，，前四字节为块地址（小端模式）
			if(spi_cmd_data.length>=5)
				W25qxx_EraseBlock(U8A2U32(&(spi_cmd_data.data[1])));
			else
				{
					spi_cmd_data.data[0]=0xff;//失败命令的返回
					spi_cmd_data.length=0x01;

				}

			break;
		case 5://5号命令，W25qxx_IsEmptyPage，检查页是否为空,地址(4字节)+偏移(4字节)+数量(4字节)，均为小端模式
			if(!(spi_cmd_data.length>=13 && W25qxx_IsEmptyPage(U8A2U32(&(spi_cmd_data.data[1])),U8A2U32(&(spi_cmd_data.data[5])),U8A2U32(&(spi_cmd_data.data[9])))))
			{
					spi_cmd_data.data[0]=0xff;//失败命令的返回
					spi_cmd_data.length=0x01;
			}

			break;
		case 6://6号命令，W25qxx_IsEmptySector,检查扇区是否为空,地址(4字节)+偏移(4字节)+数量(4字节)，均为小端模式
			if(!(spi_cmd_data.length>=13 && W25qxx_IsEmptySector(U8A2U32(&(spi_cmd_data.data[1])),U8A2U32(&(spi_cmd_data.data[5])),U8A2U32(&(spi_cmd_data.data[9])))))
			{
				spi_cmd_data.data[0]=0xff;//失败命令的返回
				spi_cmd_data.length=0x01;
			}

			break;
		case 7://7号命令，W25qxx_IsEmptyBlock，检查块是否为空,地址(4字节)+偏移(4字节)+数量(4字节)，均为小端模式
			if(!(spi_cmd_data.length>=13 && W25qxx_IsEmptyBlock(U8A2U32(&(spi_cmd_data.data[1])),U8A2U32(&(spi_cmd_data.data[5])),U8A2U32(&(spi_cmd_data.data[9])))))
			{
				spi_cmd_data.data[0]=0xff;//失败命令的返回
				spi_cmd_data.length=0x01;
			}

			break;
		}

		CDC_Transmit_FS(spi_cmd_data.data,spi_cmd_data.length);
		spi_cmd_data.IsUpdate=0;
	}
}
