#include "userapp.h"
#include "main.h"
#include "stm32f1xx_hal.h"

//导入硬件定义函数
extern I2C_HandleTypeDef hi2c1;

extern SPI_HandleTypeDef hspi1;

extern UART_HandleTypeDef huart2;

//只支持GCC
linecode_t linecode=
{
.Rate=9600,
.StopBits=0,
.Parity=0,
.DataBits=8,
.IsUpdate=0
};

WorkMode_t WorkMode=Mode_UART;

void loop_user_call()//在Main函数里循环调用此函数
{
	if(linecode.IsUpdate)
	{
		HAL_Delay(20);//延时20ms设置通信参数
		if(linecode.Rate<1500000)//速率小于1.5Mbps，为串口模式(及其有限的串口支持)
		{//重新初始化串口
			HAL_UART_DeInit(&huart2);
			huart2.Instance = USART2;

			huart2.Init.BaudRate = linecode.Rate;//linecode的速率

			huart2.Init.WordLength = UART_WORDLENGTH_8B;//只支持8/9数据位（包括校验位）
			if(linecode.DataBits==8 && linecode.Parity!=0)
			{
				huart2.Init.WordLength=UART_WORDLENGTH_9B;
			}

			huart2.Init.StopBits = UART_STOPBITS_1;
			if(linecode.StopBits==2)
			{
				huart2.Init.StopBits=UART_STOPBITS_2;
			}


			huart2.Init.Parity = UART_PARITY_NONE;
			if(linecode.Parity==1)
				huart2.Init.Parity = UART_PARITY_ODD;
			if(linecode.Parity==1)
				huart2.Init.Parity = UART_PARITY_EVEN;



			huart2.Init.Mode = UART_MODE_TX_RX;

			huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;//不支持流控

			huart2.Init.OverSampling = UART_OVERSAMPLING_16;

			 if (HAL_UART_Init(&huart2) != HAL_OK)
			  {//打开失败则使用默认设置
				  HAL_UART_DeInit(&huart2);
				  huart2.Instance = USART2;
				  huart2.Init.BaudRate = 115200;
				  huart2.Init.WordLength = UART_WORDLENGTH_8B;
				  huart2.Init.StopBits = UART_STOPBITS_1;
				  huart2.Init.Parity = UART_PARITY_NONE;
				  huart2.Init.Mode = UART_MODE_TX_RX;
				  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
				  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
				  HAL_UART_Init(&huart2);
				  linecode.DataBits=8;
				  linecode.Parity=0;
				  linecode.Rate=115200;
				  linecode.StopBits=0;
			  }
		}
		linecode.IsUpdate=0;
	}
}
