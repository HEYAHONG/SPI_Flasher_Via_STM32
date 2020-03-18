/*
 * useruart.h
 *
 *  Created on: Mar 18, 2020
 *      Author: Administrator
 */

#ifndef USERUART_H_
#define USERUART_H_
extern uint8_t uart_rx_buff[];//接收缓冲
//extern uint8_t uart_tx_buff[];//发送缓冲

void uart_receive_call(void * buf,size_t len);//接收数据完成后调用此函数
uint8_t uart_transmit(void * buf,size_t len);//发送数据到串口
void uart_start_receive();// 启动DMA传输

#endif /* USERUART_H_ */
