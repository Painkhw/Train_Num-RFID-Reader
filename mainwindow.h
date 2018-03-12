#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QFileDialog>
#include <QDesktopWidget>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void WindowInit();

    void on_pB_Open_clicked();

    void on_pB_Clear_clicked();

    void on_pB_Send_clicked();

    void SerialPortUpdate();

    void SerialErrorHandler(QSerialPort::SerialPortError);

    void ReadData();

    void on_pB_OpenFile_clicked();

    void on_pB_Save_clicked();

    void on_pB_RTClear_clicked();

    void on_checkBox_Send_clicked();

    void on_checkBox_Rec_clicked();

    QString HexToStr(QByteArray receivedata);

    QByteArray StrToHex(QString inputstr);

    //QString StrToHex(QString str);

   // void on_pB_About_clicked();

    void on_pB_Stop_clicked();


private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
