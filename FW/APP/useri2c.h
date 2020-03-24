/*
 * useri2c.h
 *
 *  Created on: Mar 23, 2020
 *      Author: Administrator
 */

#ifndef USERI2C_H_
#define USERI2C_H_
#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "eeprom24xx.h"

extern I2C_HandleTypeDef hi2c1;


extern uint8_t i2c_rx_buff[];

/*
 *   	I2C命令在data中的结构：
 *   		 命令号（1字节）+命令数据，成功返回为请求命令号+返回数据，失败返回0xff+数据(无效)
 *
 */
typedef struct
{
size_t length;
uint8_t IsUpdate;
uint8_t * data;
} i2c_cmd_data_t;

extern i2c_cmd_data_t i2c_cmd_data;

void i2c_cmd_process_call();//i2c命令模式处理函数

#endif /* USERI2C_H_ */
