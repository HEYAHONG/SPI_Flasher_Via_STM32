/*
 * useri2c.c
 *
 *  Created on: Mar 23, 2020
 *      Author: Administrator
 */
#include "userapp.h"

uint8_t i2c_rx_buff[1000];
i2c_cmd_data_t i2c_cmd_data=
{
.length=0,
.data=i2c_rx_buff,
.IsUpdate=0
};

void i2c_cmd_process_call()//i2c命令模式处理函数
{
	if(i2c_cmd_data.IsUpdate)
	{
		switch(i2c_cmd_data.data[0])
		{
		default:
			i2c_cmd_data.data[0]=0xff;
			i2c_cmd_data.length=1;
			break;
		case 1://设置目标EEPROM容量，并检测EEPROM,两字节数据（低字节在前）
			if(i2c_cmd_data.length>=3 && EEPROM24XX_IsConnected())
			{
				uint32_t size=i2c_cmd_data.data[1]+(uint32_t)i2c_cmd_data.data[2]*256;
				if(size!=0)
				{
					size_t i=0,j=0x01;
					while(!((j<<i)&size))
						i++;
					_EEPROM_SIZE_KBIT=(j<<i);
				}
				else
				{
					_EEPROM_SIZE_KBIT=64;//默认为64
				}
			}
			else
			{
				i2c_cmd_data.data[0]=0xff;
				i2c_cmd_data.length=1;
			}
			break;
		case 2://读取EEPROM，地址（2字节，低字节在前）+待读取的数据长度（两字节,低字节在前，最多512字节）

			{
				uint32_t i=i2c_cmd_data.data[3]+(uint32_t)i2c_cmd_data.data[4]*256;
				if(i==0 || i>512)
					i=512;
				if(!(i2c_cmd_data.length >=5 && EEPROM24XX_Load(i2c_cmd_data.data[1]+(uint32_t)i2c_cmd_data.data[2]*256,&i2c_cmd_data.data[1],i)))
				{
					i2c_cmd_data.data[0]=0xff;
					i2c_cmd_data.length=1;
				}
				else
				{
					i2c_cmd_data.length=1+i;
				}
			}
			break;
		case 3://写入EEPROM,地址（2字节，低字节在前）+待写入的数据
			if(i2c_cmd_data.length<=4)
			{
				i2c_cmd_data.data[0]=0xff;
				i2c_cmd_data.length=1;
				break;
			}

			if(!EEPROM24XX_Save(i2c_cmd_data.data[1]+(uint32_t)i2c_cmd_data.data[2]*256,&i2c_cmd_data.data[3],i2c_cmd_data.length-3))
			{
				i2c_cmd_data.data[0]=0xff;
				i2c_cmd_data.length=1;
			}
			break;


		}

		CDC_Transmit_FS(i2c_cmd_data.data,i2c_cmd_data.length);
		i2c_cmd_data.IsUpdate=0;
	}
}
