#include "userapp.h"



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

void init_user_call()//在硬件初始化完成后调用
{
	__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);//打开总线空闲中断
}

void loop_user_call()//在Main函数里循环调用此函数
{
	if(linecode.IsUpdate)
	{
		HAL_Delay(20);//延时20ms设置通信参数
		if(linecode.Rate<1500000)//速率小于1.5Mbps，为串口模式(极其有限的串口支持)
		{//重新初始化串口
			WorkMode=Mode_UART;
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
			if(linecode.Parity==2)
				huart2.Init.Parity = UART_PARITY_EVEN;



			huart2.Init.Mode = UART_MODE_TX_RX;

			huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;//不支持流控

			huart2.Init.OverSampling = UART_OVERSAMPLING_16;

			 if (HAL_UART_Init(&huart2) != HAL_OK)
			  {//打开失败则使用默认设置
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
			 uart_start_receive();
		}

		if(linecode.Rate>=1500000)//切换为i2c模式
		{
			WorkMode=Mode_I2C_RAW;

			if(linecode.StopBits!=0)
			{
						WorkMode=Mode_I2C_CMD;
			}
		}

		if(linecode.Rate>=2000000)//切换为spi模式
		{
			WorkMode=Mode_SPI_RAW;

			if(linecode.StopBits!=0)
			{
				WorkMode=Mode_SPI_CMD;
			}
		}
		linecode.IsUpdate=0;
	}

	if(WorkMode==Mode_SPI_CMD)
			spi_cmd_process_call();//调用spi命令处理函数（不在中断中处理）
	if(WorkMode==Mode_I2C_CMD)
			i2c_cmd_process_call();//调用i2c命令处理函数（不在中断中处理）
}

void cdc_receive_call(uint8_t* Buf, uint32_t Len)//由USB CDC/ACM接收数据时调用
{
	switch(WorkMode)
	{
	default:
		uart_transmit(Buf,Len);
		break;
	case Mode_SPI_RAW:
		CDC_Transmit_FS(spi_transmitReceive(Buf,Len),Len);
		break;
	case Mode_SPI_CMD:
		if(spi_cmd_data.IsUpdate!=0) break;//上条命令未处理，忽略下一条命令
		spi_cmd_data.length=Len;
		memcpy(spi_cmd_data.data,Buf,Len);
		spi_cmd_data.IsUpdate++;
		break;
	case Mode_I2C_RAW:
		if(Buf[0] & 0x01)//首字符表示8位器件地址(最低位表示读/写)，写入/读取长度位Len-1
		{
			HAL_I2C_Master_Receive(&hi2c1,Buf[0],&Buf[1],Len-1,1000+Len);
		}
		else
		{
			HAL_I2C_Master_Transmit(&hi2c1,Buf[0],&Buf[1],Len-1,1000+Len);
		}
		CDC_Transmit_FS(Buf,Len);
		break;
	case Mode_I2C_CMD:
		if(i2c_cmd_data.IsUpdate!=0) break;//上条命令未处理，忽略下一条命令
		i2c_cmd_data.length=Len;
		memcpy(i2c_cmd_data.data,Buf,Len);
		i2c_cmd_data.IsUpdate++;
		break;

	}
}

void uart_receive_call(void * buf,size_t len)//接收数据完成后调用此函数
{
	if(WorkMode!=Mode_UART)
		return;//检查模式
	CDC_Transmit_FS(buf,len);//发送给上位机
}
