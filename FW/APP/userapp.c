#include "userapp.h"
#include "main.h"
#include "stm32f1xx_hal.h"
//只支持GCC
linecode_t linecode=
{
.Rate=9600,
.StopBits=0,
.Parity=0,
.DataBits=8,
.IsUpdate=0
};

void loop_user_call()//在Main函数里循环调用此函数
{
	if(linecode.IsUpdate)
	{
		HAL_Delay(20);//延时20ms设置通信参数

		linecode.IsUpdate=0;
	}
}
