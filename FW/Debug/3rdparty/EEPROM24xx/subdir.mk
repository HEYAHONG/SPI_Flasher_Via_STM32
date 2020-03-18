################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../3rdparty/EEPROM24xx/eeprom24xx.c 

OBJS += \
./3rdparty/EEPROM24xx/eeprom24xx.o 

C_DEPS += \
./3rdparty/EEPROM24xx/eeprom24xx.d 


# Each subdirectory must supply rules for building sources it contributes
3rdparty/EEPROM24xx/eeprom24xx.o: ../3rdparty/EEPROM24xx/eeprom24xx.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F103xB -DDEBUG -c -I../USB_DEVICE/Target -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../USB_DEVICE/App -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../APP -I"D:/windows_10_software/Users/Administrator/Desktop/SPI_Flasher_Via_STM32/FW/3rdparty/EEPROM24xx" -I"D:/windows_10_software/Users/Administrator/Desktop/SPI_Flasher_Via_STM32/FW/3rdparty/w25qxx" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"3rdparty/EEPROM24xx/eeprom24xx.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

