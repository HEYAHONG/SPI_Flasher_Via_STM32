#include "serialport.h"

//构造/析构函数
SerialPort::SerialPort(QString _serialport)
{
    serialport=_serialport;
    IsOpen=false;
#ifndef WIN32
    fd=-1;
#else
    hSerial=NULL;
#endif
}

SerialPort::~SerialPort()
{
    if(IsOpen)
    {
        close(true);
    }
}

//#################################################
//打开串口
bool SerialPort::open(QString _seriaport)
{
    if(_seriaport.length() !=0)
        serialport=_seriaport;
    if(serialport.length() ==0)
        return false;
    if(!IsOpen && serialport.length()!=0)
    {

#ifdef WIN32

    DWORD  accessdirection =GENERIC_READ | GENERIC_WRITE;
    hSerial = CreateFileA((char*)serialport.toStdString().c_str(),
                accessdirection,
                0,
                0,
                OPEN_EXISTING,
                0,
                0);
            if (hSerial == INVALID_HANDLE_VALUE)
            {
                IsOpen=false;
            }
            else
            {
                IsOpen=true;
            }
         //清空缓冲区
          if(IsOpen)
              PurgeComm(hSerial,PURGE_TXABORT |PURGE_RXABORT |PURGE_TXCLEAR|PURGE_RXCLEAR);
          SetupComm(hSerial,1024,1024);

#else

        fd = ::open((const char*)serialport.toStdString().c_str(),O_RDWR|  O_NOCTTY | O_NONBLOCK);
        if(fd >= 0)
        {
            fcntl(fd, F_SETFL, 0);//设置为阻塞模式
            IsOpen=true;
        }

#endif
    }
    return IsOpen;
}

bool SerialPort::close(bool force)
{
  #ifdef WIN32
  if(force)
  {
    CloseHandle(hSerial);
    IsOpen=false;
  }
  else if(IsOpen)
  {
      CloseHandle(hSerial);
      IsOpen=false;
  }
  #else
    if(force)
    {
        if(fd >=0)
            ::close(fd);
        IsOpen=false;
    }
    else
    {
        if(fd >=0)
          {
            if(::close(fd)>=0)
            {
                IsOpen=false;
            }
          }
    }
  #endif
    return !IsOpen;
}

#ifndef WIN32
struct speed_map
{
  const char *string;		/* ASCII representation. */
  speed_t speed;		/* Internal form. */
  unsigned long value;		/* Numeric value. */
};
#endif

bool SerialPort::setup(int speed, int data_bits, int parity, int stop_bits)
{
#ifdef WIN32
    if(!IsOpen)
        return false;
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        return false;
    }
    dcbSerialParams.BaudRate=speed;
    dcbSerialParams.ByteSize=data_bits;
    //dcbSerialParams.StopBits=stopbits;
    //dcbSerialParams.Parity=parity;
    /* parity bits */
    switch (parity)
    {
    case 'N':
    case 'n':
      dcbSerialParams.Parity=NOPARITY;
        break;
    case 'O':
    case 'o':
     dcbSerialParams.Parity= ODDPARITY;
        break;
    case 'E':
    case 'e':
      dcbSerialParams.Parity=EVENPARITY;
        break;
    default:
       return false;
    }

    /* stop bits */
    switch (stop_bits)
    {
    case 1:
       dcbSerialParams.StopBits=ONESTOPBIT;
        break;
    case 2:
       dcbSerialParams.StopBits=TWOSTOPBITS;
        break;
    default:
        return false;
    }
    if(!SetCommState(hSerial, &dcbSerialParams)){
         return false;
    }
    COMMTIMEOUTS timeouts={0};
    timeouts.ReadIntervalTimeout=50;
    timeouts.ReadTotalTimeoutConstant=2000;
    timeouts.ReadTotalTimeoutMultiplier=100;
    timeouts.WriteTotalTimeoutConstant=50;
    timeouts.WriteTotalTimeoutMultiplier=10;
    if(!SetCommTimeouts(hSerial, &timeouts)){
        return false;
    }
#else

    struct speed_map speeds[] =
    {
      {"0", B0, 0},
      {"50", B50, 50},
      {"75", B75, 75},
      {"110", B110, 110},
      {"134", B134, 134},
      {"134.5", B134, 134},
      {"150", B150, 150},
      {"200", B200, 200},
      {"300", B300, 300},
      {"600", B600, 600},
      {"1200", B1200, 1200},
      {"1800", B1800, 1800},
      {"2400", B2400, 2400},
      {"4800", B4800, 4800},
      {"9600", B9600, 9600},
      {"19200", B19200, 19200},
      {"38400", B38400, 38400},
      {"exta", B19200, 19200},
      {"extb", B38400, 38400},
    #ifdef B57600
      {"57600", B57600, 57600},
    #endif
    #ifdef B115200
      {"115200", B115200, 115200},
    #endif
    #ifdef B230400
      {"230400", B230400, 230400},
    #endif
    #ifdef B460800
      {"460800", B460800, 460800},
    #endif
    #ifdef B500000
      {"500000", B500000, 500000},
    #endif
    #ifdef B576000
      {"576000", B576000, 576000},
    #endif
    #ifdef B921600
      {"921600", B921600, 921600},
    #endif
    #ifdef B1000000
      {"1000000", B1000000, 1000000},
    #endif
    #ifdef B1152000
      {"1152000", B1152000, 1152000},
    #endif
    #ifdef B1500000
      {"1500000", B1500000, 1500000},
    #endif
    #ifdef B2000000
      {"2000000", B2000000, 2000000},
    #endif
    #ifdef B2500000
      {"2500000", B2500000, 2500000},
    #endif
    #ifdef B3000000
      {"3000000", B3000000, 3000000},
    #endif
    #ifdef B3500000
      {"3500000", B3500000, 3500000},
    #endif
    #ifdef B4000000
      {"4000000", B4000000, 4000000},
    #endif
      {NULL, 0, 0}
    };

    int ret=0;

    if(fd < 0)
        return false;

    struct termios opt;
    if(tcgetattr(fd, &opt) <0)
        return false;
    opt.c_cflag |= (CLOCAL | CREAD); /* enable the receiver, set local mode */
    opt.c_cflag &= ~CSIZE;	/* mask the character size bits*/

    {
        size_t i=0;
        while(i<sizeof(speeds)/sizeof (speeds[0]))
        {
           if(speeds[i].value >= (unsigned long)speed)
               break;
           i++;
        }

        cfsetispeed(&opt, speeds[i].speed);
        cfsetospeed(&opt, speeds[i].speed);
    }


    {//设置特殊波特率
       // struct serial_struct serial_set;
        //if((ioctl(fd,TIOCGSERIAL,&serial_set))<0)
        //{
        //       return false;
        //}

        //tcflush(fd,TCIFLUSH);/*handle unrecevie char*/
        //tcsetattr(fd,TCSANOW,&opt);

        //serial_set.flags = ASYNC_SPD_CUST;
        //serial_set.custom_divisor = serial_set.baud_base / speed;

        //if((ioctl(fd,TIOCSSERIAL,&serial_set))<0)
        //{
          //  qDebug()<<"BAUD: error to set serial_struct:\n"<<strerror(errno);
           // return false;
        //}

    }

    /* data bits */
        switch (data_bits)
        {
        case 8:
            opt.c_cflag |= CS8;
            break;
        case 7:
            opt.c_cflag |= CS7;
            break;
        default:
            return false;
        }

        /* parity bits */
        switch (parity)
        {
        case 'N':
        case 'n':
            opt.c_iflag &= ~INPCK;
            opt.c_cflag &= ~PARENB;
            break;
        case 'O':
        case 'o':
            opt.c_iflag |= (INPCK|ISTRIP); /*enable parity check, strip parity bits*/
            opt.c_cflag |= (PARODD | PARENB);
            break;
        case 'E':
        case 'e':
            opt.c_iflag |= (INPCK|ISTRIP); /*enable parity check, strip parity bits*/
            opt.c_cflag |= PARENB;
            opt.c_cflag &= ~PARODD;
            break;
        default:
           return false;
        }

        /* stop bits */
        switch (stop_bits)
        {
        case 1:
            opt.c_cflag &= ~CSTOPB;
            break;
        case 2:
            opt.c_cflag |= CSTOPB;
            break;
        default:
            return false;
        }

        /* raw input */
            opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
            /* raw ouput */
            opt.c_oflag &= ~OPOST;

            //设置流控
            //RTS/CTS (硬件) 流控制
            opt.c_cflag &= ~CRTSCTS; //无流控
            //输入的 XON/XOFF 流控制
            opt.c_iflag &= ~IXOFF;//不启用
            //输出的 XON/XOFF 流控制
            opt.c_iflag &= ~IXON ;//不启用

            // 防止0x0D变0x0A
            opt.c_iflag &= ~(ICRNL|IGNCR);

            tcflush(fd, TCIFLUSH);
            opt.c_cc[VTIME] = 50; /* time out */
            opt.c_cc[VMIN] = 0; /* minimum number of characters to read */

            ret = tcsetattr(fd, TCSANOW, &opt); /* update it now */
            if (ret < 0)
                return false;
#endif
        return true;
}

int SerialPort::read(uint8_t * buff,size_t length)
{
  int ret=-1;
#ifdef WIN32
  if(!IsOpen)
      return -1;
   PurgeComm(hSerial,PURGE_TXABORT |PURGE_RXABORT |PURGE_TXCLEAR|PURGE_RXCLEAR);
  DWORD dwBytesRead = 0;
  if(!ReadFile(hSerial, buff, length, &dwBytesRead, NULL)){
        return -1;
     }
     return (int)dwBytesRead;
#else
  if(IsOpen)
  {
      ret=::read(fd,buff,length);
      if(ret < 0)
          close(true);

  }
#endif
  return ret;
}

int SerialPort::write(uint8_t * buff,size_t length)
{
  int ret=-1;
#ifdef WIN32
  if(!IsOpen)
      return -1;
   PurgeComm(hSerial,PURGE_TXABORT |PURGE_RXABORT |PURGE_TXCLEAR|PURGE_RXCLEAR);
     DWORD dwBytesRead = 0;
      if(!WriteFile(hSerial, buff, length, &dwBytesRead, NULL)){
         return -1;
      }
      return (int)dwBytesRead;
#else
  if(IsOpen)
  {
      ret=::write(fd,buff,length);
      if(ret < 0)
          close(true);

  }
#endif
  return ret;
}
