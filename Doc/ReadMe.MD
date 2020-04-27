本工程为一个USB烧录器，主要功能为烧录SPI Flash和I2C EEPROM。对V1.0.0的固件进行了改进。修改如下:

* 删除了用于调试的附属功能

* 将USB CDC/ACM换为USB MASS STORAGE CLASS，加快读写速度


# 软件部分

采用[VirtualFat](https://github.com/HEYAHONG/STM32_VirtualFat.git),虚拟一个U盘，将SPI Flash和I2C EEPROM像普通文件一样读。当硬件连接正确且上位机正确识别U盘后，在U盘中会出现24XXX.bin与25XXX.bin,对这些文件的操作会映射到SPI Flash与I2C EEPROM上。

## 支持的操作系统

* windows


# 硬件部分

硬件部分主要由一块带USB接口的主板和各种封装转DIP-8座组成。

![主板图片](USB_Flasher_Main.jpg)

注意:主板外形会随着PCB工艺，选择性焊接和硬件设计版本的不同而不同，但最主要的USB接口和插座接口均不会缺少。

由于有很多EEPROM和SPI Flash并不是DIP-8封装的，因此需要转接插座才能安装到主板上。

> SOP-8转DIP-8插座（主要用于安装w25q80）：

>> ![SOP-8座](SOP-8_2_DIP-8.jpg)

> TSSOP-8转DIP-8座(主要用于安装AT24C64)：

>> ![TSSOP-8座](TSSOP-8_2_DIP-8.jpg)


注意:芯片安装时，1脚的位置要正确，否则可能烧坏芯片（接反时，芯片电源和地会对调）
