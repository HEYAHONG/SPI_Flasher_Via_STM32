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

    //寻找flasher
    find_flasher();

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

void MainWindow::find_flasher()
{
#ifdef WIN32
    for(char i='A';i<='Z';i++)
    {
        QFile file;
        file.setFileName(QString(i)+":/FSINFO.TXT");
        if(file.exists())
        {
            ui->lineEdit_path->setText(QString(i)+":/");
        }
    }
#endif
}

void MainWindow::on_lineEdit_path_textEdited(const QString &arg1)
{
    QString path=QFileDialog::getExistingDirectory(this,"设置FSINFO.TXT所在目录",arg1);
    if(path.length()>0)
    {
        ui->lineEdit_path->setText(path);
    }
}

  void MainWindow::fsinfo_txt_write(QString cmd)
  {
      QString fsinfo_path=((QString)ui->lineEdit_path->text()+"/FSINFO.TXT");
      QFile file;
      file.setFileName(fsinfo_path);
      if(!file.exists())
      {
          QMessageBox::warning(this,"错误","FSINFO.TXT目录设置不正确!");
          return;
      }
      else
      {
          if(file.open(QFile::WriteOnly))
          {
              file.write(cmd.toStdString().c_str());
              file.close();
          }

      }
  }

void MainWindow::on_pushButton_clicked()//重启单片机
{
    fsinfo_txt_write("RESET");
}

bool MainWindow::flash_op_con_check()//flash操作条件检测，满足条件返回真
{
    bool ret=true;
    {//检测25QXX文件是否存在
        QFile file;
        file.setFileName(ui->lineEdit_path->text()+"/25QXX.BIN");
        if(!file.exists())
        {
            QMessageBox::warning(this,"错误","找不到SPI Flash");
            ret=false;
        }
    }
    return ret;
}

void MainWindow::on_pushButton_Flash_Erase_clicked()
{
    fsinfo_txt_write("FE");
}

void MainWindow::on_pushButton_Flash_Read_clicked()
{
    if(flash_op_con_check())
    {
        QFile file;
        file.setFileName(ui->lineEdit_path->text()+"/25QXX.BIN");
        if(file.open(QFile::ReadOnly))
        {
            QByteArray data,read_buff;
            quint64 read_size=0;
            do
            {
                read_buff=file.read(8192);//8K一次读
                data+=read_buff;
                read_size+=read_buff.length();
                ui->statusbar->showMessage(QString("已读取")+QString::number(read_size*100.00/file.size()).left(5)+"%");
            } while(read_buff.length()==8192);

            ui->Edit->setData(data);
            file.close();
        }
    }
}

void MainWindow::on_pushButton_Flash_Write_clicked()
{
    if(flash_op_con_check())
    {
        QFile file;
        int64_t filesize;
        file.setFileName(ui->lineEdit_path->text()+"/25QXX.BIN");
        filesize=file.size();
        if(file.open(QFile::WriteOnly))
        {
            QByteArray data=ui->Edit->data();

            if(data.length()>filesize)
                data=data.left(filesize);//只写入file的大小

            while (data.length()>0)
            {
                char buff[512];//按512字节写，按其他字节写可能出现多余的0
                memset(buff,0xff,sizeof(buff));
                {
                    memcpy(buff,data.toStdString().c_str(),data.size()>512?512:data.size());
                }
                file.write(buff,sizeof (buff));

                data.remove(0,sizeof(buff));
            }

            file.close();
        }
    }
}

void MainWindow::on_pushButton_Flash_Check_clicked()
{
    if(flash_op_con_check())
    {
        QFile file;
        file.setFileName(ui->lineEdit_path->text()+"/25QXX.BIN");
        if(file.open(QFile::ReadOnly))
        {
            QByteArray data,read_buff;
            quint64 read_size=0;
            do
            {
                read_buff=file.read(8192);//8K一次读
                data+=read_buff;
                read_size+=read_buff.length();
                ui->statusbar->showMessage(QString("已读取")+QString::number(read_size*100.00/file.size()).left(5)+"%");
            } while(read_buff.length()==8192);

            CheckData(data);
            file.close();
        }
    }
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    if(arg1.toUInt()<=0)
    {
        ui->lineEdit->setText("64");
    }
}

void MainWindow::on_pushButton_EEPROM_size_clicked()
{
    fsinfo_txt_write(QString("E")+ui->lineEdit->text().left(3));
}

QString MainWindow::Get_EEPROM_FileName()//获得EEPROM的文件名
{
    QString ret=ui->lineEdit_path->text()+"/"+"24X"+ui->lineEdit->text().left(3)+".BIN";
    QFile file;
    file.setFileName(ret);
    if(!file.exists())
    {
        QMessageBox::warning(this,"错误","找不到EEPROM");
        ret="";
    }
    return ret;
}

void MainWindow::on_pushButton_EEPROM_read_clicked()
{
    QString path=Get_EEPROM_FileName();
    if(path.length()>0)
    {
        QFile file;
        file.setFileName(path);
        if(file.open(QFile::ReadOnly))
        {
            ui->Edit->setData(file.readAll());
            file.close();
        }
    }
}

void MainWindow::on_pushButton_EEPROM_check_clicked()
{
    QString path=Get_EEPROM_FileName();
    if(path.length()>0)
    {
        QFile file;
        file.setFileName(path);
        if(file.open(QFile::ReadOnly))
        {
            CheckData(file.readAll());
            file.close();
        }
    }
}

void MainWindow::on_pushButton_EEPROM_write_clicked()
{
    QString path=Get_EEPROM_FileName();
    if(path.length()>0)
    {
        QFile file;
        int64_t filesize;
        file.setFileName(path);
        filesize=file.size();
        if(file.open(QFile::WriteOnly))
        {
            QByteArray data=ui->Edit->data();
            if(data.length()>filesize)
            {
                data=data.left(filesize);
            }
            do
            {
               char buff[512];
               memset(buff,0xff,sizeof (buff));
               memcpy(buff,data.toStdString().c_str(),data.length()>512?512:data.length());
               file.write(buff,sizeof(buff));

               data.remove(0,sizeof(buff));

            } while(data.length()>=512);
            file.close();
        }
    }
}
