/*
 * userspi.c
 *
 *  Created on: Mar 18, 2020
 *      Author: Administrator
 */

#include "userapp.h"

static uint8_t spi_rx_buff[1000];

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
