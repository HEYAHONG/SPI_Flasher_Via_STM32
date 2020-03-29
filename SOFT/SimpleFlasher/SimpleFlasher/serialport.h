#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <QMainWindow>
#include <QDebug>

#ifndef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#endif

class SerialPort
{
public:
    SerialPort(QString _serialport="");
    ~SerialPort();

    bool IsOpen;
    QString serialport;

#ifndef WIN32
    int fd;
#endif

    //打开串口
    bool open(QString _seriaport="");
    //关闭串口
    bool close(bool force=false);
    //配置串口
    bool setup(int speed, int data_bits, int parity, int stop_bits);
    //读串口
    int read(uint8_t * buff,size_t length);
    //写串口
    int write(uint8_t * buff,size_t length);

};

#endif // SERIALPORT_H
