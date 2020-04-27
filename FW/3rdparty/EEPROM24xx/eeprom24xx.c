#include "main.h"
#include "eeprom24xx.h"
#if (_EEPROM_FREERTOS_IS_ENABLE==1)
#include "cmsis_os.h"
#endif

size_t _EEPROM_SIZE_KBIT=64;//容量设置
//##########################################################################
bool		EEPROM24XX_IsConnected(void)
{
	#if	(_EEPROM_USE_WP_PIN==1)
	HAL_GPIO_WritePin(_EEPROM_WP_GPIO,_EEPROM_WP_PIN,GPIO_PIN_SET);
	#endif
	if(HAL_I2C_IsDeviceReady(&_EEPROM24XX_I2C,0xa0,1,100)==HAL_OK)
		return true;
	else
		return false;	
}
//##########################################################################
//不跨页写
static bool		EEPROM24XX_Save_No_Page(uint16_t Address,void *data,size_t size_of_data)
{
	uint8_t ret=0;
	if ((_EEPROM_SIZE_KBIT==1) || (_EEPROM_SIZE_KBIT==2))
		{
			if(size_of_data > 8)
			return false;
		}
	else if ((_EEPROM_SIZE_KBIT==4) || (_EEPROM_SIZE_KBIT==8) || (_EEPROM_SIZE_KBIT==16))
		 {
			if(size_of_data > 16)
			return false;
		 }
	else
		 {
			if(size_of_data > 32)
			return false;
		 }
	
	#if	(_EEPROM_USE_WP_PIN==1)
	HAL_GPIO_WritePin(_EEPROM_WP_GPIO,_EEPROM_WP_PIN,GPIO_PIN_RESET);
	#endif
	
	if ((_EEPROM_SIZE_KBIT==1) || (_EEPROM_SIZE_KBIT==2))
		{
			if(HAL_I2C_Mem_Write(&_EEPROM24XX_I2C,0xa0,Address,I2C_MEMADD_SIZE_8BIT,(uint8_t*)data,size_of_data,100) == HAL_OK) ret=1;
		}
	else if	(_EEPROM_SIZE_KBIT==4)
		{
			if(HAL_I2C_Mem_Write(&_EEPROM24XX_I2C,0xa0|((Address&0x0100>>7)),(Address&0xff),I2C_MEMADD_SIZE_8BIT,(uint8_t*)data,size_of_data,100) == HAL_OK) ret=1;
		}
	else if	(_EEPROM_SIZE_KBIT==8)
		{
			if(HAL_I2C_Mem_Write(&_EEPROM24XX_I2C,0xa0|((Address&0x0300>>7)),(Address&0xff),I2C_MEMADD_SIZE_8BIT,(uint8_t*)data,size_of_data,100) == HAL_OK) ret=1;
		}
	else if(_EEPROM_SIZE_KBIT==16)
		{
			if(HAL_I2C_Mem_Write(&_EEPROM24XX_I2C,0xa0|((Address&0x0700>>7)),(Address&0xff),I2C_MEMADD_SIZE_8BIT,(uint8_t*)data,size_of_data,100) == HAL_OK) ret=1;
		}
	else
		{
			if(HAL_I2C_Mem_Write(&_EEPROM24XX_I2C,0xa0,Address,I2C_MEMADD_SIZE_16BIT,(uint8_t*)data,size_of_data,100) == HAL_OK) ret=1;
		}


	if(ret)
	{
		#if (_EEPROM_FREERTOS_IS_ENABLE==1)
		osDelay(7);
		#else
		HAL_Delay(7);
		#endif
		#if	(_EEPROM_USE_WP_PIN==1)
		HAL_GPIO_WritePin(_EEPROM_WP_GPIO,_EEPROM_WP_PIN,GPIO_PIN_SET);
		#endif
		return true;
	}
	else
	{
		#if	(_EEPROM_USE_WP_PIN==1)
		HAL_GPIO_WritePin(_EEPROM_WP_GPIO,_EEPROM_WP_PIN,GPIO_PIN_SET);
		#endif
		return false;		
	}
}

//##########################################################################
bool		EEPROM24XX_Save(uint16_t Address,void *data,size_t size_of_data)
{
	bool ret=true;

	//判断页大小
	size_t page_size=32;
	if ((_EEPROM_SIZE_KBIT==1) || (_EEPROM_SIZE_KBIT==2))
			{
				page_size = 8;

			}
	else if ((_EEPROM_SIZE_KBIT==4) || (_EEPROM_SIZE_KBIT==8) || (_EEPROM_SIZE_KBIT==16))
			 {
				page_size = 16;
			 }
	else
			 {
				page_size = 32;
			 }

	size_t data_to_write=size_of_data,data_written=0;
	uint8_t * buff=(uint8_t *)data;

	//写开始一页的数据
	data_written=(Address/page_size+1)*page_size-Address;
	if(data_written > data_to_write) data_written=data_to_write;
	ret &= EEPROM24XX_Save_No_Page(Address,buff,data_written);
	data_to_write-=data_written;

	//写中间页
	while(data_to_write>page_size)
	{
		ret &=EEPROM24XX_Save_No_Page(Address+size_of_data-data_to_write,buff+size_of_data-data_to_write,page_size);
		data_to_write-=page_size;
	}

	//写最后一页
	ret &=EEPROM24XX_Save_No_Page(Address+size_of_data-data_to_write,buff+size_of_data-data_to_write,data_to_write);

	return ret;
}

//##########################################################################
bool		EEPROM24XX_Load(uint16_t Address,void *data,size_t size_of_data)
{

	uint8_t ret=0;
	#if	(_EEPROM_USE_WP_PIN==1)
	HAL_GPIO_WritePin(_EEPROM_WP_GPIO,_EEPROM_WP_PIN,GPIO_PIN_SET);
	#endif

	if ((_EEPROM_SIZE_KBIT==1) || (_EEPROM_SIZE_KBIT==2))
		{
			if(HAL_I2C_Mem_Read(&_EEPROM24XX_I2C,0xa0,Address,I2C_MEMADD_SIZE_8BIT,(uint8_t*)data,size_of_data,100) == HAL_OK) ret=1;
		}
	else if (_EEPROM_SIZE_KBIT==4)
		{
		 	 if(HAL_I2C_Mem_Read(&_EEPROM24XX_I2C,0xa0|((Address&0x0100>>7)),(Address&0xff),I2C_MEMADD_SIZE_8BIT,(uint8_t*)data,size_of_data,100) == HAL_OK) ret=1;
		}
	else if (_EEPROM_SIZE_KBIT==8)
		{
			if(HAL_I2C_Mem_Read(&_EEPROM24XX_I2C,0xa0|((Address&0x0300>>7)),(Address&0xff),I2C_MEMADD_SIZE_8BIT,(uint8_t*)data,size_of_data,100) == HAL_OK) ret=1;
		}
	else if (_EEPROM_SIZE_KBIT==16)
		{
			if(HAL_I2C_Mem_Read(&_EEPROM24XX_I2C,0xa0|((Address&0x0700>>7)),(Address&0xff),I2C_MEMADD_SIZE_8BIT,(uint8_t*)data,size_of_data,100) == HAL_OK) ret=1;
		}
	else
		{
			if(HAL_I2C_Mem_Read(&_EEPROM24XX_I2C,0xa0,Address,I2C_MEMADD_SIZE_16BIT,(uint8_t*)data,size_of_data,100) == HAL_OK) ret=1;
		}


	if(ret)
		return true;
	else
		return false;		
}
