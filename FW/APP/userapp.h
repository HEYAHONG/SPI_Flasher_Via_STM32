#ifndef __USERAPP_H__
#define __USERAPP_H__
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "main.h"
#include "stm32f1xx_hal.h"

#include "useruart.h"

//导入硬件定义函数
extern I2C_HandleTypeDef hi2c1;

extern SPI_HandleTypeDef hspi1;

extern UART_HandleTypeDef huart2;

//Line Code 结构体，由CDC_Control_FS传入
typedef struct
{
	uint32_t Rate;
	uint8_t  StopBits;
	uint8_t  Parity;
	uint16_t  DataBits;
	uint8_t  IsUpdate;//是否更新计数
} linecode_t;

//临时存储linecode数据
extern linecode_t linecode;

//工作模式变量
typedef enum {
	Mode_UART=0,
} WorkMode_t;

extern WorkMode_t WorkMode;

void init_user_call();//在硬件初始化完成后调用
void loop_user_call();//在Main函数里循环调用此函数

void cdc_receive_call(uint8_t* Buf, uint32_t Len);//由USB CDC/ACM接收数据时调用
extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);//CDC/ACM发送函数


#define __USERAPP_H__

#endif
