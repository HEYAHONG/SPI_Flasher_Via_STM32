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

//拉高CS
#define SPI_CS_HIGH() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)
//拉低CS
#define SPI_CS_LOW() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)

void * spi_transmitReceive(void * txbuff,size_t len);

#endif /* USERSPI_H_ */
