#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowTitle("SimpleFlasher");

    // 添加搜索框
    searchdialog=new SearchDialog(ui->Edit,this);

    //更新串口号
    serialport="";
    UpdateCom();
    //连接信号/槽
    connect(ui->menu,SIGNAL(triggered(QAction *)),this,SLOT(menu_triggered(QAction *)));
    connect(ui->menu_2,SIGNAL(triggered(QAction *)),this,SLOT(menu_2_triggered(QAction *)));
    connect(ui->menu_3,SIGNAL(triggered(QAction *)),this,SLOT(menu_3_triggered(QAction *)));


}

MainWindow::~MainWindow()
{
    delete searchdialog;
    delete ui;
}

//hex存储函数
extern "C"
{
static void memory_desc_store_qt(void *arg, int address, uint8_t c)
{
    struct memory_desc *mi = (struct memory_desc *)arg;

    //自动使用读取的第一个地址作为偏移地址，要求hex文件中地址从小到大排列
    static int8_t flag=-1;
    if(flag==-1)
    {
        mi->offset=address;
        flag=1;
    }



    if (address >= mi->offset && address < mi->offset + mi->size) {
        int offset = address - mi->offset;
        mi->p[offset] = c;
        if (offset + 1 > mi->size_written) {
            mi->size_written = offset + 1;
        }
    }
}
}

void 	MainWindow::menu_triggered(QAction *action)
{

    if(action==ui->action)//打开
    {
       static QFile file;
       QString path=QFileDialog::getOpenFileName(this,"","");
       if(path.length()==0)
       {
           ui->statusbar->showMessage("打开操作取消！");
           return;
       }

        file.setFileName(path);
        ui->Edit->setData(file);
        if(file.isOpen())
            file.close();
        if(path.right(4).compare(".hex",Qt::CaseInsensitive)== 0)
        {//检测到是hex文件
           uint64_t file_size=file.size();
           uint8_t  buffer[file_size];
           struct memory_desc md;
           memory_desc_init(&md, buffer,0, file_size);
           {
             FILE * fp=fopen(path.toStdString().c_str(),"r");
             if(fp!=NULL)
             {
                 if(read_hex(fp, memory_desc_store_qt, &md,1))
                 {
                     static QBuffer qbuffer(this);
                     qbuffer.setData((char *)buffer,md.size_written);
                     ui->Edit->setData(qbuffer);
                 }
                 fclose(fp);
             }
           }
        }
    }
    if(action==ui->action_2)//保存
    {
       static QFile file;
       QString path=QFileDialog::getSaveFileName(this,"","","bin文件 (*.bin)");
       if(path.length()==0)
       {
           ui->statusbar->showMessage("保存操作取消！");
           return;
       }
       if(file.isOpen())
           file.close();
       file.setFileName(path);
       file.open(QIODevice::ReadWrite);
       file.write(ui->Edit->data());
       file.close();

    }
    if(action==ui->action_5)//退出
    {
        MainWindow::close();
    }

    ui->statusbar->showMessage("操作完成！");
}
void 	MainWindow::menu_2_triggered(QAction *action)
{
    if(action==ui->action_7)//恢复上次操作
    {
        ui->Edit->redo();
    }
    if(action==ui->action_8)//撤销上次操作
    {
        ui->Edit->undo();
    }

    if(action==ui->action_10)//搜索框
    {
        if(action->isChecked())
                searchdialog->show();
        else
                searchdialog->hide();
    }

    ui->statusbar->showMessage("操作完成！");
}
void 	MainWindow::menu_3_triggered(QAction *action)
{
    (void)action;//避免警告
    QMessageBox::information(this,"关于","SimpleFlasher by HYH");
}


void MainWindow::UpdateCom()
{
        ui->com->clear();

       {//添加串口名
           foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
           {
               qDebug()<<"Name:"<<info.portName();
               qDebug()<<"Description:"<<info.description();
               qDebug()<<"Manufacturer:"<<info.manufacturer();

               //这里相当于自动识别串口号之后添加到了cmb，如果要手动选择可以用下面列表的方式添加进去
               QSerialPort serial;
               serial.setPort(info);
               if(serial.open(QIODevice::ReadWrite))
               {
                   //将串口号添加到cmb
                   ui->com->addItem(info.portName());
                   //关闭串口等待人为(打开串口按钮)打开
                   serial.close();
               }
           }
         }
}

void MainWindow::on_com_currentIndexChanged(const QString &arg1)
{

    serialport=arg1;
#ifndef WIN32
    serialport=QString("/dev/")+arg1;
#else
    serialport=QString("\\\\.\\")+arg1;
#endif
}

void MainWindow::on_pushButton_clicked()
{
    UpdateCom();
}

void MainWindow::on_radioButton_spi_toggled(bool checked)
{
   if(checked)
   {
    ui->flash_size->setText("8192");
    ui->pushButton_4->setEnabled(false);
    ui->pushButton_5->setEnabled(true);
    ui->pushButton_6->setEnabled(true);
   }
}

void MainWindow::on_radioButton_i2c_toggled(bool checked)
{
   if(checked)
   {
    ui->flash_size->setText("64");
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(false);
    ui->pushButton_6->setEnabled(false);
   }
}

void MainWindow::on_flash_size_textChanged(const QString &arg1)
{
    bool is_success=false;
    arg1.toInt(&is_success);
    if(!is_success)
        ui->flash_size->setText("64");
}

void MainWindow::on_pushButton_2_clicked()
{
    if(serialport.length()==0)
    {
        ui->statusbar->showMessage("串口设置错误\n");
        QMessageBox::warning(this,"警告","串口设置错误\n");
        return;
    }

    int flash_size=ui->flash_size->text().toInt();
    if(flash_size<=0 || flash_size >= 8*1024*512)
    {
        ui->statusbar->showMessage("大小设置错误\n");
        QMessageBox::warning(this,"警告","大小设置错误\n");
        return;
    }

    SerialPort sp;
    if(!sp.open(serialport))
    {
        ui->statusbar->showMessage("串口打开错误\n");
        QMessageBox::warning(this,"警告","串口打开错误\n");
        return;
    }


    if(ui->radioButton_spi->isChecked())//类型为spi_flash
    {
        uint8_t write_buff[64]={0},read_buff[64]={0};

        if(!sp.setup(2000000,8,'N',2))
        {
            ui->statusbar->showMessage("串口参数设置错误\n");
            QMessageBox::warning(this,"警告","串口参数错误\n");
            return;
        }

        QThread::msleep(50);//等待模式切换完成
        int address=0; QByteArray data;

        {//初始化SPI Flash通信
            //填写1号命令
            write_buff[0]=0x01;
            //填写读缓冲
            read_buff[0]=0xff;
            if(sp.write(write_buff,1) < 1)
            {
                ui->statusbar->showMessage("串口读写错误\n");
                QMessageBox::warning(this,"警告","串口读写错误\n");
                return;
            }
            if(sp.read(read_buff,41) < 41)
            {
                ui->statusbar->showMessage("串口读写错误\n");
                QMessageBox::warning(this,"警告","串口读写错误\n");
                return;
            }
            if(read_buff[0]!=write_buff[0])
            {
                ui->statusbar->showMessage("SPI Flash未正确安装\n");
                QMessageBox::warning(this,"警告","SPI Flash未正确安装\n");
                return;
            }
            else
            {
                ui->statusbar->showMessage("初始化SPI Flash\n");
                //通过ID设置flash大小
                if(read_buff[1]>0)
                {
                    flash_size=1024*(1<<(read_buff[1]-1));
                    ui->flash_size->setText(QString::number(flash_size));
                }
                repaint();


            }
        };

        flash_size*=(1024/8);// 转化为字节数

        //读取数据
        while(address < flash_size)
        {
            int bytetoread=0;
            if((flash_size-address)<32)
            {
                bytetoread=flash_size-address;
            }
            else
            {
                bytetoread=32;
            }

            write_buff[0]=13;
            write_buff[1]=(address&0xff);
            write_buff[2]=(address>>8)&0xff;
            write_buff[3]=(address>>16)&0xff;
            write_buff[4]=(address>>24)&0xff;
            write_buff[5]=bytetoread;
            write_buff[6]=0;
            write_buff[7]=0;
            write_buff[8]=0;

            read_buff[0]=0xff;
            if(sp.write(write_buff,9) < 9 || sp.read(read_buff,9+bytetoread) < 9+bytetoread)
            {
                ui->statusbar->showMessage("串口读写错误\n");
                QMessageBox::warning(this,"警告","串口读写错误\n");
                return;
            }
            if(read_buff[0]!=write_buff[0])
            {
                ui->statusbar->showMessage("读取失败\n");
                QMessageBox::warning(this,"警告","读取失败\n");
                return;
            }
            else
            {
                double tmp=(double)(address+bytetoread)/flash_size;
                ui->statusbar->showMessage(QString::number(tmp*100,'g',4)+"%已读取\n");
                //ui->statusbar->showMessage(QString::number(address,16)+"读取"+QString::number(bytetoread)+"字节成功\n");
                {
                 static double last_tmp=0;
                 if(tmp-last_tmp>0.001)
                    {
                     repaint();
                     last_tmp=tmp;
                    }
                 if(last_tmp>=0.9999)
                 {
                     last_tmp=0;
                 }
                }
            }
            address+=bytetoread;
            data+=QByteArray((char *)&read_buff[9],bytetoread);
        }

        static QBuffer buff;
        buff.setData(data);
        ui->Edit->setData(buff);

    }

    if(ui->radioButton_i2c->isChecked())// 类型为I2c_EEPROM
    {
        uint8_t write_buff[64]={0},read_buff[64]={0};

        if(!sp.setup(1500000,8,'N',2))
        {
            ui->statusbar->showMessage("串口参数设置错误\n");
            QMessageBox::warning(this,"警告","串口参数错误\n");
            return;
        }

        QThread::msleep(50);//等待模式切换完成

        int address=0; QByteArray data;
        {
            {//设置EEPROM大小
                //填写1号命令
                write_buff[0]=0x01;
                write_buff[1]=(flash_size & 0xff);
                write_buff[2]=((flash_size & 0xff00)>>8);
                //填写读缓冲
                read_buff[0]=0xff;
                if(sp.write(write_buff,3) < 3 || sp.read(read_buff,3) < 3)
                {
                    ui->statusbar->showMessage("串口读写错误\n");
                    QMessageBox::warning(this,"警告","串口读写错误\n");
                    return;
                }
                if(read_buff[0]!=write_buff[0])
                {
                    ui->statusbar->showMessage("EEPROM未正确安装\n");
                    QMessageBox::warning(this,"警告","EEPROM未正确安装\n");
                    return;
                }
                else
                {
                    ui->statusbar->showMessage("设置EEPROM大小\n");
                    repaint();
                }
            };

            flash_size*=(1024/8);// 转化为字节数

            //读取数据
            while(address < flash_size)
            {
                int bytetoread=0;
                if((flash_size-address)<32)
                {
                    bytetoread=flash_size-address;
                }
                else
                {
                    bytetoread=32;
                }

                write_buff[0]=0x02;
                write_buff[1]=(address&0xff);
                write_buff[2]=(address>>8);
                write_buff[3]=bytetoread;
                write_buff[4]=0;

                read_buff[0]=0xff;
                if(sp.write(write_buff,5) < 5 || sp.read(read_buff,1+bytetoread) < 1+bytetoread)
                {
                    ui->statusbar->showMessage("串口读写错误\n");
                    QMessageBox::warning(this,"警告","串口读写错误\n");
                    return;
                }
                if(read_buff[0]!=write_buff[0])
                {
                    ui->statusbar->showMessage("读取失败\n");
                    QMessageBox::warning(this,"警告","读取失败\n");
                    return;
                }
                else
                {
                    double tmp=(double)(address+bytetoread)/flash_size;
                    ui->statusbar->showMessage(QString::number(tmp*100,'g',4)+"%已读取\n");
                    //ui->statusbar->showMessage(QString::number(address,16)+"读取"+QString::number(bytetoread)+"字节成功\n");
                    {
                     static double last_tmp=0;
                     if(tmp-last_tmp>0.001)
                        {
                         repaint();
                         last_tmp=tmp;
                        }
                     if(last_tmp>=0.9999)
                     {
                         last_tmp=0;
                     }
                    }
                }
                address+=bytetoread;
                data+=QByteArray((char *)&read_buff[1],bytetoread);
            }

            static QBuffer buff;
            buff.setData(data);
            ui->Edit->setData(buff);
        }
    }

    sp.close(true);
}

void MainWindow::CheckData(QByteArray data)
{
    QByteArray edit_data=ui->Edit->data();
    if(data.length()==0 || edit_data.length()==0)
    {
        ui->statusbar->showMessage("待校验数据或者读出数据为0，校验退出!\n");
        QMessageBox::warning(this,"警告","待校验数据或者读出数据为0，校验退出!\n");
        return;
    }

    if(data.length() != edit_data.length())
    {
        ui->statusbar->showMessage("待校验数据与读出数据长度不相等，结果可能有误\n");
        QMessageBox::warning(this,"警告","待校验数据与读出数据长度不相等，结果可能有误\n");
    }

    int length=data.length()>edit_data.length()?edit_data.length():data.length();
    for(int i=0;i<length;i++)
    {
        if(edit_data[i]!= data[i])
        {
            ui->statusbar->showMessage("校验出错:\n"+QString::number(i,16)+"处所在数据不相等!\n");
            QMessageBox::warning(this,"警告","校验出错:\n"+QString::number(i,16)+"处所在数据不相等!\n");
            return;
        }
        else
        {
            double tmp=(double)i/length;
            ui->statusbar->showMessage(QString::number(tmp*100,'g',4)+"%已校验\n");
            {
             static double last_tmp=0;
             if(tmp-last_tmp>0.001)
                {
                 repaint();
                 last_tmp=tmp;
                }
             if(last_tmp>=0.9999)
             {
                 last_tmp=0;
             }
            }

        }
    }

    ui->statusbar->showMessage("校验完成");

}

void MainWindow::on_pushButton_3_clicked()
{
    if(serialport.length()==0)
    {
        ui->statusbar->showMessage("串口设置错误\n");
        QMessageBox::warning(this,"警告","串口设置错误\n");
        return;
    }

    int flash_size=ui->flash_size->text().toInt();
    if(flash_size<=0 || flash_size >= 8*1024*512)
    {
        ui->statusbar->showMessage("大小设置错误\n");
        QMessageBox::warning(this,"警告","大小设置错误\n");
        return;
    }

    SerialPort sp;
    if(!sp.open(serialport))
    {
        ui->statusbar->showMessage("串口打开错误\n");
        QMessageBox::warning(this,"警告","串口打开错误\n");
        return;
    }


    if(ui->radioButton_spi->isChecked())//类型为spi_flash
    {
        uint8_t write_buff[64]={0},read_buff[64]={0};

        if(!sp.setup(2000000,8,'N',2))
        {
            ui->statusbar->showMessage("串口参数设置错误\n");
            QMessageBox::warning(this,"警告","串口参数错误\n");
            return;
        }

        QThread::msleep(50);//等待模式切换完成
        int address=0; QByteArray data;

        {//初始化SPI Flash通信
            //填写1号命令
            write_buff[0]=0x01;
            //填写读缓冲
            read_buff[0]=0xff;
            if(sp.write(write_buff,1) < 1 || sp.read(read_buff,41) < 41)
            {
                ui->statusbar->showMessage("串口读写错误\n");
                QMessageBox::warning(this,"警告","串口读写错误\n");
                return;
            }
            if(read_buff[0]!=write_buff[0])
            {
                ui->statusbar->showMessage("SPI Flash未正确安装\n");
                QMessageBox::warning(this,"警告","SPI Flash未正确安装\n");
                return;
            }
            else
            {
                ui->statusbar->showMessage("初始化SPI Flash\n");
                //通过ID设置flash大小
                if(read_buff[1]>0)
                {
                    flash_size=1024*(1<<(read_buff[1]-1));
                    ui->flash_size->setText(QString::number(flash_size));
                }
                repaint();


            }
        };

        flash_size*=(1024/8);// 转化为字节数

        //读取数据
        while(address < flash_size)
        {
            int bytetoread=0;
            if((flash_size-address)<32)
            {
                bytetoread=flash_size-address;
            }
            else
            {
                bytetoread=32;
            }

            write_buff[0]=13;
            write_buff[1]=(address&0xff);
            write_buff[2]=(address>>8)&0xff;
            write_buff[3]=(address>>16)&0xff;
            write_buff[4]=(address>>24)&0xff;
            write_buff[5]=bytetoread;
            write_buff[6]=0;
            write_buff[7]=0;
            write_buff[8]=0;

            read_buff[0]=0xff;
            if(sp.write(write_buff,9) < 9 || sp.read(read_buff,9+bytetoread) < 9+bytetoread)
            {
                ui->statusbar->showMessage("串口读写错误\n");
                QMessageBox::warning(this,"警告","串口读写错误\n");
                return;
            }
            if(read_buff[0]!=write_buff[0])
            {
                ui->statusbar->showMessage("读取失败\n");
                QMessageBox::warning(this,"警告","读取失败\n");
                return;
            }
            else
            {
                double tmp=(double)(address+bytetoread)/flash_size;
                ui->statusbar->showMessage(QString::number(tmp*100,'g',4)+"%已读取\n");
                //ui->statusbar->showMessage(QString::number(address,16)+"读取"+QString::number(bytetoread)+"字节成功\n");
                {
                 static double last_tmp=0;
                 if(tmp-last_tmp>0.001)
                    {
                     repaint();
                     last_tmp=tmp;
                    }
                 if(last_tmp>=0.9999)
                 {
                     last_tmp=0;
                 }
                }
            }
            address+=bytetoread;
            data+=QByteArray((char *)&read_buff[9],bytetoread);
        }

         CheckData(data);

    }

    if(ui->radioButton_i2c->isChecked())// 类型为I2c_EEPROM
    {
        uint8_t write_buff[64]={0},read_buff[64]={0};

        if(!sp.setup(1500000,8,'N',2))
        {
            ui->statusbar->showMessage("串口参数设置错误\n");
            QMessageBox::warning(this,"警告","串口参数错误\n");
            return;
        }

        QThread::msleep(50);//等待模式切换完成

        int address=0; QByteArray data;
        {
            {//设置EEPROM大小
                //填写1号命令
                write_buff[0]=0x01;
                write_buff[1]=(flash_size & 0xff);
                write_buff[2]=((flash_size & 0xff00)>>8);
                //填写读缓冲
                read_buff[0]=0xff;
                if(sp.write(write_buff,3) < 3 || sp.read(read_buff,3) < 3)
                {
                    ui->statusbar->showMessage("串口读写错误\n");
                    QMessageBox::warning(this,"警告","串口读写错误\n");
                    return;
                }
                if(read_buff[0]!=write_buff[0])
                {
                    ui->statusbar->showMessage("EEPROM未正确安装\n");
                    QMessageBox::warning(this,"警告","EEPROM未正确安装\n");
                    return;
                }
                else
                {
                    ui->statusbar->showMessage("设置EEPROM大小\n");
                    repaint();
                }
            };

            flash_size*=(1024/8);// 转化为字节数

            //读取数据
            while(address < flash_size)
            {
                int bytetoread=0;
                if((flash_size-address)<32)
                {
                    bytetoread=flash_size-address;
                }
                else
                {
                    bytetoread=32;
                }

                write_buff[0]=0x02;
                write_buff[1]=(address&0xff);
                write_buff[2]=(address>>8);
                write_buff[3]=bytetoread;
                write_buff[4]=0;

                read_buff[0]=0xff;
                if(sp.write(write_buff,5) < 5 || sp.read(read_buff,1+bytetoread) < 1+bytetoread)
                {
                    ui->statusbar->showMessage("串口读写错误\n");
                    QMessageBox::warning(this,"警告","串口读写错误\n");
                    return;
                }
                if(read_buff[0]!=write_buff[0])
                {
                    ui->statusbar->showMessage("读取失败\n");
                    QMessageBox::warning(this,"警告","读取失败\n");
                    return;
                }
                else
                {
                    double tmp=(double)(address+bytetoread)/flash_size;
                    ui->statusbar->showMessage(QString::number(tmp*100,'g',4)+"%已读取\n");
                    //ui->statusbar->showMessage(QString::number(address,16)+"读取"+QString::number(bytetoread)+"字节成功\n");
                    {
                     static double last_tmp=0;
                     if(tmp-last_tmp>0.001)
                        {
                         repaint();
                         last_tmp=tmp;
                        }
                     if(last_tmp>=0.9999)
                     {
                         last_tmp=0;
                     }
                    }
                }
                address+=bytetoread;
                data+=QByteArray((char *)&read_buff[1],bytetoread);
            }

            CheckData(data);
        }
    }

    sp.close(true);
}

void MainWindow::on_pushButton_4_clicked()
{
    if(serialport.length()==0)
    {
        ui->statusbar->showMessage("串口设置错误\n");
        QMessageBox::warning(this,"警告","串口设置错误\n");
        return;
    }

    int flash_size=ui->flash_size->text().toInt();
    if(flash_size<=0 || flash_size >= 8*1024*512)
    {
        ui->statusbar->showMessage("大小设置错误\n");
        QMessageBox::warning(this,"警告","大小设置错误\n");
        return;
    }

    SerialPort sp;
    if(!sp.open(serialport))
    {
        ui->statusbar->showMessage("串口打开错误\n");
        QMessageBox::warning(this,"警告","串口打开错误\n");
        return;
    }

    uint8_t write_buff[64]={0},read_buff[64]={0};

    if(!sp.setup(1500000,8,'N',2))
    {
        ui->statusbar->showMessage("串口参数设置错误\n");
        QMessageBox::warning(this,"警告","串口参数错误\n");
        return;
    }

    QThread::msleep(50);//等待模式切换完成

    int address=0; QByteArray data=ui->Edit->data();

    {//设置EEPROM大小
        //填写1号命令
        write_buff[0]=0x01;
        write_buff[1]=(flash_size & 0xff);
        write_buff[2]=((flash_size & 0xff00)>>8);
        //填写读缓冲
        read_buff[0]=0xff;
        if(sp.write(write_buff,3) < 3 || sp.read(read_buff,3) < 3)
        {
            ui->statusbar->showMessage("串口读写错误\n");
            QMessageBox::warning(this,"警告","串口读写错误\n");
            return;
        }
        if(read_buff[0]!=write_buff[0])
        {
            ui->statusbar->showMessage("EEPROM未正确安装\n");
            QMessageBox::warning(this,"警告","EEPROM未正确安装\n");
            return;
        }
        else
        {
            ui->statusbar->showMessage("设置EEPROM大小\n");
            repaint();
        }
    };

    flash_size*=(1024/8);// 转化为字节数

    //调整写入大小
    if(flash_size>data.length())
            flash_size=data.length();

    //写入数据
    while(address < flash_size)
    {
        int bytetowrite=0;
        if((flash_size-address)<32)
        {
            bytetowrite=flash_size-address;
        }
        else
        {
           bytetowrite=32;
        }

        write_buff[0]=0x03;
        write_buff[1]=(address&0xff);
        write_buff[2]=(address>>8);
        memcpy(&write_buff[3],&data.toStdString().c_str()[address],bytetowrite);

        read_buff[0]=0xff;
        if(sp.write(write_buff,3+bytetowrite) < 3+bytetowrite || sp.read(read_buff,3+bytetowrite) < 3+bytetowrite)
        {
            ui->statusbar->showMessage("串口读写错误\n");
            QMessageBox::warning(this,"警告","串口读写错误\n");
            return;
        }
        if(read_buff[0]!=write_buff[0])
        {
            ui->statusbar->showMessage("写入失败\n");
            QMessageBox::warning(this,"警告","写入失败\n");
            return;
        }
        else
        {
            double tmp=(double)(address+bytetowrite)/flash_size;
            ui->statusbar->showMessage(QString::number(tmp*100,'g',4)+"%已写入\n");
            {
             static double last_tmp=0;
             if(tmp-last_tmp>0.001)
                {
                 repaint();
                 last_tmp=tmp;
                }
             if(last_tmp>=0.9999)
                {
                 last_tmp=0;
                }
                }
         }

        address+=bytetowrite;
    }


    sp.close();
}

void MainWindow::on_pushButton_5_clicked()
{
    if(serialport.length()==0)
    {
        ui->statusbar->showMessage("串口设置错误\n");
        QMessageBox::warning(this,"警告","串口设置错误\n");
        return;
    }

    int flash_size=ui->flash_size->text().toInt();
    if(flash_size<=0 || flash_size >= 8*1024*512)
    {
        ui->statusbar->showMessage("大小设置错误\n");
        QMessageBox::warning(this,"警告","大小设置错误\n");
        return;
    }

    SerialPort sp;
    if(!sp.open(serialport))
    {
        ui->statusbar->showMessage("串口打开错误\n");
        QMessageBox::warning(this,"警告","串口打开错误\n");
        return;
    }

    uint8_t write_buff[64]={0},read_buff[64]={0};

    if(!sp.setup(2000000,8,'N',2))
    {
        ui->statusbar->showMessage("串口参数设置错误\n");
        QMessageBox::warning(this,"警告","串口参数错误\n");
        return;
    }

    QThread::msleep(50);//等待模式切换完成

    {//初始化SPI Flash通信
        //填写1号命令
        write_buff[0]=0x01;
        //填写读缓冲
        read_buff[0]=0xff;
        if(sp.write(write_buff,1) < 1 || sp.read(read_buff,41) < 41)
        {
            ui->statusbar->showMessage("串口读写错误\n");
            QMessageBox::warning(this,"警告","串口读写错误\n");
            return;
        }
        if(read_buff[0]!=write_buff[0])
        {
            ui->statusbar->showMessage("SPI Flash未正确安装\n");
            QMessageBox::warning(this,"警告","SPI Flash未正确安装\n");
            return;
        }
        else
        {
            ui->statusbar->showMessage("初始化SPI Flash\n");
            //通过ID设置flash大小
            if(read_buff[1]>0)
            {
                flash_size=1024*(1<<(read_buff[1]-1));
                ui->flash_size->setText(QString::number(flash_size));
            }
            repaint();


        }
    };

    {//擦除spi flash
        //填写2号命令
        write_buff[0]=0x02;
        //填写读缓冲
        read_buff[0]=0xff;
        if(sp.write(write_buff,3) < 1 )
        {
            ui->statusbar->showMessage("串口读写错误\n");
            QMessageBox::warning(this,"警告","串口读写错误\n");
            return;
        }

        QThread::sleep(3);

        sp.read(read_buff,3);
        if(read_buff[0]!=write_buff[0])
        {
            ui->statusbar->showMessage("擦除失败\n");
            QMessageBox::warning(this,"警告","擦除失败\n");
            return;
        }
        else
        {
            ui->statusbar->showMessage("擦除成功\n");
            repaint();


        }
    };




    sp.close();
}

void MainWindow::on_pushButton_6_clicked()
{
    if(serialport.length()==0)
    {
        ui->statusbar->showMessage("串口设置错误\n");
        QMessageBox::warning(this,"警告","串口设置错误\n");
        return;
    }

    int flash_size=ui->flash_size->text().toInt();
    if(flash_size<=0 || flash_size >= 8*1024*512)
    {
        ui->statusbar->showMessage("大小设置错误\n");
        QMessageBox::warning(this,"警告","大小设置错误\n");
        return;
    }

    SerialPort sp;
    if(!sp.open(serialport))
    {
        ui->statusbar->showMessage("串口打开错误\n");
        QMessageBox::warning(this,"警告","串口打开错误\n");
        return;
    }

    uint8_t write_buff[64]={0},read_buff[64]={0};

    if(!sp.setup(2000000,8,'N',2))
    {
        ui->statusbar->showMessage("串口参数设置错误\n");
        QMessageBox::warning(this,"警告","串口参数错误\n");
        return;
    }

    QThread::msleep(50);//等待模式切换完成

    {//初始化SPI Flash通信
        //填写1号命令
        write_buff[0]=0x01;
        //填写读缓冲
        read_buff[0]=0xff;
        if(sp.write(write_buff,1) < 1 || sp.read(read_buff,41) < 41)
        {
            ui->statusbar->showMessage("串口读写错误\n");
            QMessageBox::warning(this,"警告","串口读写错误\n");
            return;
        }
        if(read_buff[0]!=write_buff[0])
        {
            ui->statusbar->showMessage("SPI Flash未正确安装\n");
            QMessageBox::warning(this,"警告","SPI Flash未正确安装\n");
            return;
        }
        else
        {
            ui->statusbar->showMessage("初始化SPI Flash\n");
            //通过ID设置flash大小
            if(read_buff[1]>0)
            {
                flash_size=1024*(1<<(read_buff[1]-1));
                ui->flash_size->setText(QString::number(flash_size));
            }
            repaint();


        }
    };

    flash_size*=(1024/8);// 转化为字节数



    {
        int pageaddress=0,page_offset=0,bytetowrite=0;
        QByteArray data=ui->Edit->data();
        //调整写入大小
        if(flash_size>data.length())
                flash_size=data.length();

        while(pageaddress*256+page_offset < flash_size)
        {
           //调整写入大小
           if(flash_size-(pageaddress*256+page_offset) < 32 )
           {
               bytetowrite=flash_size-(pageaddress*256+page_offset);
           }
           else
           {
               bytetowrite=32;
           }
           //不可跨页
           if(page_offset+bytetowrite > 256)
           {
               bytetowrite=256-page_offset;
           }

           {//按页写命令
               write_buff[0]=9;
               write_buff[1]=(pageaddress)&0xff;
               write_buff[2]=(pageaddress>>8)&0xff;
               write_buff[3]=(pageaddress>>16)&0xff;
               write_buff[4]=(pageaddress>>24)&0xff;
               write_buff[5]=(page_offset)&0xff;
               write_buff[6]=(page_offset>>8)&0xff;
               write_buff[7]=(page_offset>>16)&0xff;
               write_buff[8]=(page_offset>>24)&0xff;
               memcpy(&write_buff[9],&data.toStdString().c_str()[pageaddress*256+page_offset],bytetowrite);

               read_buff[0]=0xff;
               if(sp.write(write_buff,9+bytetowrite) < 9+bytetowrite || sp.read(read_buff,9+bytetowrite) < 9+bytetowrite)
               {
                   ui->statusbar->showMessage("串口读写错误\n");
                   QMessageBox::warning(this,"警告","串口读写错误\n");
                   return;
               }
               if(read_buff[0]!=write_buff[0])
               {
                   ui->statusbar->showMessage("写入失败\n");
                   QMessageBox::warning(this,"警告","写入失败\n");
                   return;
               }
               else
               {
                   double tmp=(double)(pageaddress*256+page_offset+bytetowrite)/flash_size;
                   ui->statusbar->showMessage(QString::number(tmp*100,'g',4)+"%已写入\n");
                   {
                    static double last_tmp=0;
                    if(tmp-last_tmp>0.001)
                       {
                        repaint();
                        last_tmp=tmp;
                       }
                    if(last_tmp>=0.9999)
                    {
                        last_tmp=0;
                    }
                   }
               }

           }

           //增加地址
           if(page_offset+bytetowrite >= 256)
           {
               pageaddress+=1;
               page_offset=0;
           }
           else
           {
               page_offset+=bytetowrite;
           }

        }

    }



    sp.close();
}
