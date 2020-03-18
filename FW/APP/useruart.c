/*
 * useruart.c
 *
 *  Created on: Mar 18, 2020
 *      Author: Administrator
 */


#include "userapp.h"
uint8_t uart_rx_buff[1000];//接收缓冲
//uint8_t uart_tx_buff[1000];//发送缓冲
uint8_t uart_transmit(void * buf,size_t len)//发送数据到串口
{
	HAL_UART_Transmit(&huart2,buf,len,(uint32_t)(len*(1000.0/linecode.Rate))+1000);
	return HAL_OK;
}

void uart_start_receive()
{
	if(huart2.gState !=HAL_UART_STATE_READY && huart2.gState != HAL_UART_STATE_BUSY_TX)
			HAL_UART_DMAStop(&huart2);
	if(huart2.gState == HAL_UART_STATE_BUSY_TX)
			while(huart2.gState!=HAL_UART_STATE_READY);//等待发送完成
	HAL_UART_Receive_DMA(&huart2,uart_rx_buff,sizeof(uart_rx_buff));
}

void uart_finish_receive()
{
	if(huart2.gState !=HAL_UART_STATE_READY && huart2.gState != HAL_UART_STATE_BUSY_TX)
		HAL_UART_DMAStop(&huart2);
	if(huart2.gState == HAL_UART_STATE_BUSY_TX)
		while(huart2.gState!=HAL_UART_STATE_READY);//等待发送完成
	size_t count=sizeof(uart_rx_buff)-huart2.hdmarx->Instance->CNDTR;
	 uart_receive_call(uart_rx_buff,count);
	HAL_UART_DMAStop(&huart2);
	HAL_UART_Receive_DMA(&huart2,uart_rx_buff,sizeof(uart_rx_buff));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)//DMA接收全满
{
	 UNUSED(huart);
	 uart_finish_receive();
}
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)//DMA接收半满
{
	 UNUSED(huart);
	 uart_finish_receive();
}



