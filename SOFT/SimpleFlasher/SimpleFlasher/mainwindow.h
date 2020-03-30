#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qhexedit.h"
#include "readhex.h"
#include "QMessageBox"
#include "QMenu"
#include "QFile"
#include "QFileDialog"
#include "searchdialog.h"
#include "QSerialPort"
#include "QSerialPortInfo"
#include "serialport.h"

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
    void UpdateCom();
    QString serialport;
    void CheckData(QByteArray data);
public slots:
    void 	menu_triggered(QAction *action);
    void 	menu_2_triggered(QAction *action);
    void 	menu_3_triggered(QAction *action);
private slots:
    void on_com_currentIndexChanged(const QString &arg1);
    void on_pushButton_clicked();
    void on_radioButton_spi_toggled(bool checked);
    void on_radioButton_i2c_toggled(bool checked);
    void on_flash_size_textChanged(const QString &arg1);
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
};
#endif // MAINWINDOW_H
