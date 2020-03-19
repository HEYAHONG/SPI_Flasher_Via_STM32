/*
 * userspi.h
 *
 *  Created on: Mar 18, 2020
 *      Author: Administrator
 */

#ifndef USERSPI_H_
#define USERSPI_H_
#include "stdint.h"
#include "string.h"

/*
 *   	spi命令在data中的结构：
 *   		 命令号（1字节）+命令数据，成功返回为请求命令号+返回数据，失败返回0xff+数据(无效)
 *
 */
typedef struct
{
size_t length;
uint8_t IsUpdate;
uint8_t * data;
} spi_cmd_data_t;

extern spi_cmd_data_t spi_cmd_data;

//拉高CS
#define SPI_CS_HIGH() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)
//拉低CS
#define SPI_CS_LOW() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)

void * spi_transmitReceive(void * txbuff,size_t len);
void spi_cmd_process_call();//spi命令模式处理函数

#endif /* USERSPI_H_ */
