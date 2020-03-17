#ifndef __USERAPP_H__
#define __USERAPP_H__
#include "stdint.h"
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

void loop_user_call();//在Main函数里循环调用此函数

#define __USERAPP_H__

#endif
