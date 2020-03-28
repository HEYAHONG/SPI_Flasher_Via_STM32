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

void 	MainWindow::menu_triggered(QAction *action)
{

    if(action==ui->action)//打开
    {
       static QFile file;
       QString path=QFileDialog::getOpenFileName(this,"","hex/bin文件 (*.hex | *.bin)");
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
           memory_desc_init(&md, buffer,0x0000, file_size);
           {
             FILE * fp=fopen(path.toStdString().c_str(),"r");
             if(fp!=NULL)
             {
                 if(read_hex(fp, memory_desc_store, &md,1))
                 {
                     static QBuffer qbuffer(this);
                     qbuffer.setData((char *)buffer,md.size_written);
                     ui->Edit->setData(qbuffer);
                 }
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
}

void MainWindow::on_pushButton_clicked()
{
    UpdateCom();
}
