#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qhexedit.h"
#include "QMessageBox"
#include "QMenu"
#include "QFile"
#include "QFileDialog"
#include "searchdialog.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    SearchDialog * searchdialog;

    void CheckData(QByteArray data);

    void fsinfo_txt_write(QString cmd);

    bool flash_op_con_check();//flash操作条件检测，满足条件返回真

    void find_flasher();//寻找flasher

    QString Get_EEPROM_FileName();//获得EEPROM的文件名

public slots:
    void 	menu_triggered(QAction *action);
    void 	menu_2_triggered(QAction *action);
    void 	menu_3_triggered(QAction *action);
private slots:
    void on_lineEdit_path_textEdited(const QString &arg1);
    void on_pushButton_clicked();
    void on_pushButton_Flash_Erase_clicked();
    void on_pushButton_Flash_Read_clicked();
    void on_pushButton_Flash_Write_clicked();
    void on_pushButton_Flash_Check_clicked();
    void on_lineEdit_textChanged(const QString &arg1);
    void on_pushButton_EEPROM_size_clicked();
    void on_pushButton_EEPROM_read_clicked();
    void on_pushButton_EEPROM_check_clicked();
    void on_pushButton_EEPROM_write_clicked();
};
#endif // MAINWINDOW_H
