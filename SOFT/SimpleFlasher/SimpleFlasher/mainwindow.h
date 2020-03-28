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
public slots:
    void 	menu_triggered(QAction *action);
    void 	menu_2_triggered(QAction *action);
    void 	menu_3_triggered(QAction *action);
};
#endif // MAINWINDOW_H
