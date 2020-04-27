#ifndef	_24XXCONFIG_H
#define	_24XXCONFIG_H
#include "stdint.h"
#include "main.h"
extern I2C_HandleTypeDef hi2c1;

//#define		_EEPROM_SIZE_KBIT										32
extern  size_t _EEPROM_SIZE_KBIT;
#define		_EEPROM24XX_I2C											hi2c1			
#define		_EEPROM_FREERTOS_IS_ENABLE								0
#define		_EEPROM_USE_WP_PIN										0

#if (_EEPROM_USE_WP_PIN==1)
#define		_EEPROM_WP_GPIO											EE_WP_GPIO_Port
#define		_EEPROM_WP_PIN											EE_WP_Pin
#endif

#endif

