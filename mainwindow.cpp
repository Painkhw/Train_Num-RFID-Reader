#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTableWidget>
#include <QTableWidgetItem>


QSerialPort my_serial;
QTimer my_serialtimer;

bool open_flag = false;
bool sendhex_flag = false;
bool rechex_flag = false;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    WindowInit();
}

MainWindow::~MainWindow()
{
    delete ui;
    /* free the used class */
    my_serial.destroyed();
    my_serialtimer.destroyed();
    /* if serialport open close the serialport */ 
    if(open_flag) my_serial.close();
}

void MainWindow::WindowInit()
{
    SerialPortUpdate();
    connect(&my_serial, SIGNAL(readyRead()), SLOT(ReadData()));
    connect(&my_serial, SIGNAL(error(QSerialPort::SerialPortError)),
            SLOT(SerialErrorHandler(QSerialPort::SerialPortError)));
    /* set 1000ms refresh the serialports info and update display */
    connect(&my_serialtimer, SIGNAL(timeout()), SLOT(SerialPortUpdate()));
    my_serialtimer.start(1000);
    /* set the window center on the screen */
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight,Qt::AlignCenter,
                                          this->size(),qApp->desktop()->availableGeometry()));
    /* set the tE_SendData Focus */
    ui->tE_SendData->setFocus();

}

void MainWindow::on_pB_Open_clicked()
{
    if(!open_flag)
    {
        my_serial.setPortName(ui->cB_Devices->currentText());
        if(my_serial.open(QIODevice::ReadWrite))
        {
            open_flag = true;
            ui->statusBar->showMessage(tr("devices open successful"));
            ui->pB_Open->setText(tr("Close"));
            my_serial.setBaudRate(ui->cB_BaudRate->currentText().toULong());
            switch(ui->cB_DataBits->currentIndex())
            {
                case 0:my_serial.setDataBits(QSerialPort::Data8);break;
                case 1:my_serial.setDataBits(QSerialPort::Data7);break;
                case 2:my_serial.setDataBits(QSerialPort::Data6);break;
                case 3:my_serial.setDataBits(QSerialPort::Data5);break;
            }
            switch(ui->cB_StopBits->currentIndex())
            {
                case 0:my_serial.setStopBits(QSerialPort::OneStop);break;
                case 1:my_serial.setStopBits(QSerialPort::TwoStop);break;
            }
            switch(ui->cB_Parity->currentIndex())
            {
                case 0:my_serial.setParity(QSerialPort::NoParity);break;
                case 1:my_serial.setParity(QSerialPort::EvenParity);break;
                case 2:my_serial.setParity(QSerialPort::OddParity);break;
                case 3:my_serial.setParity(QSerialPort::SpaceParity);break;
                case 4:my_serial.setParity(QSerialPort::MarkParity);break;
            }
        }
        else
        {
            open_flag = false;
            ui->pB_Open->setText(tr("Open"));
            ui->statusBar->showMessage(tr("devices open failed"));
        }
    }
    else
    {
        open_flag = false;
        ui->pB_Open->setText(tr("Open"));
        ui->statusBar->showMessage(tr("devices closed"));
        my_serial.close();
    }

}

void MainWindow::on_pB_Stop_clicked()
{
    if(open_flag)
    {
        my_serial.clear();
    }
}

void MainWindow::on_pB_Clear_clicked()
{
   //ui->/*tE_RecData*/->clear();
}

void MainWindow::on_pB_Send_clicked()
{
    qint64 TempTxNum = 0;

    if(open_flag)
    {
        if(sendhex_flag)
        {

            TempTxNum = my_serial.write(StrToHex(ui->tE_SendData->toPlainText()));
        }
        else
        {
            //TempTxNum = my_serial.write(ui->tE_SendData->toPlainText().toLocal8Bit);
        }

        TempTxNum = TempTxNum + ui->lE_TxNumber->text().toDouble();
        ui->lE_TxNumber->setText(QString::number(TempTxNum));
    }
}

void MainWindow::ReadData()
{
    qint64 TempRxNum = 0;
    QByteArray tempbytearray;
    QString readstr;
    QString trainInfo;
    int i;

    tempbytearray = my_serial.readAll();
    readstr = QString(tempbytearray);
    TempRxNum = ui->lE_RxNumber->text().toDouble() + tempbytearray.length();
    ui->lE_RxNumber->setText(QString::number(TempRxNum));

    for(auto it = tempbytearray.begin()+2; it<tempbytearray.end()-5;it++)
            {
                *it = *it + 0x20;
                trainInfo.push_back((QString)*it);
            }

    if(rechex_flag)
    {
        /* display the receive data by hex */
       ui->tE_RecData->insertPlainText(HexToStr(tempbytearray));

       if(tempbytearray[1] == 0x07)
       {
           for(i=0; i< ui->tableWidget->rowCount();i++)
           {
               if(trainInfo == ui->tableWidget->item(i,0)->text())
               {
                   break;
               }
           }

           if(i == ui->tableWidget->rowCount())
           {

               ui->tableWidget->insertRow(ui->tableWidget->rowCount());
               QTableWidgetItem *item = new QTableWidgetItem(trainInfo);
               ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, item);

           }
       }
    }
    else
    {
       ui->tE_RecData->insertPlainText(trainInfo);
    }
}

void MainWindow::SerialPortUpdate()
{
    QString Temp_Str;

    /* save the last serialport */
    Temp_Str = ui->cB_Devices->currentText();
    ui->cB_Devices->clear();
    /* refresh the serialports */
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->cB_Devices->addItem(info.portName());
    }
    if(ui->cB_Devices->count())
    {
        /* find the last serialport */
        if(-1 != ui->cB_Devices->findText(Temp_Str))
        {
            ui->cB_Devices->setCurrentIndex(ui->cB_Devices->findText(Temp_Str));
        }
        /* if not find then display the first serialport */
        else
        {
            ui->cB_Devices->setCurrentIndex(0);
        }
    }
}

void MainWindow::SerialErrorHandler(QSerialPort::SerialPortError serialerror)
{
    ui->statusBar->showMessage(tr("serial error %1").arg(serialerror));
    if(open_flag)
    {
        ui->pB_Open->setText(tr("Open"));
        open_flag = false;
        my_serial.close();
    }
    SerialPortUpdate();
}

void MainWindow::on_pB_OpenFile_clicked()
{
    QFile open_file;
    QString filename;
    /* get the filename */
    filename = QFileDialog::getOpenFileName(this, tr("Open File"), "/home/Document", tr("all(*)"));
    /* if selectd the file */
    if(false == filename.isEmpty())
    {
        open_file.setFileName(filename);
        if(open_file.open(QIODevice::ReadOnly))
        {
            ui->tE_SendData->setPlainText(QString(open_file.readAll()));
            open_file.close();
        }
        else
        {
            ui->statusBar->showMessage(tr("open file failed"));
        }
    }
}

void MainWindow::on_pB_Save_clicked()
{
    QFile save_file;
    QString filename;

    filename = QFileDialog::getSaveFileName(this, tr("Save File"), "/home/Document", tr("all(.)"));
    if(false == filename.isEmpty())
    {
        save_file.setFileName(filename);
        if(save_file.open(QIODevice::WriteOnly))
        {
            save_file.write(ui->tE_RecData->toPlainText().toLatin1());
            save_file.close();
        }
        else
        {
            ui->statusBar->showMessage(tr("open file failed"));
        }
    }
}

void MainWindow::on_pB_RTClear_clicked()
{
    ui->lE_RxNumber->setText("0");
    ui->lE_TxNumber->setText("0");
}

void MainWindow::on_checkBox_Send_clicked()
{
    if(false == sendhex_flag)
    {
        ui->tE_SendData->setPlainText(StrToHex(ui->tE_SendData->toPlainText()));
        sendhex_flag = true;
    }
    else
    {
        ui->tE_SendData->setPlainText(HexToStr(ui->tE_SendData->toPlainText().toLocal8Bit()));
        sendhex_flag = false;
    }
}

void MainWindow::on_checkBox_Rec_clicked()
{
    if(false == rechex_flag)
    {
        ui->tE_RecData->setPlainText(StrToHex(ui->tE_RecData->toPlainText()));
        rechex_flag = true;
    }
    else
    {
        ui->tE_RecData->setPlainText(HexToStr(ui->tE_RecData->toPlainText().toLocal8Bit()));
        rechex_flag = false;
    }
}



QByteArray MainWindow::StrToHex(QString inputstr)
{
    QByteArray hexData;
    QString TempData;
    bool ok;

    for(auto it = inputstr.begin(); it != inputstr.end();it++)
    {
        if(*it != ' ')
        {
            TempData.append(*it++);

            if(*it == ' ')
            {
                hexData.append(TempData.toInt(&ok, 16));
            }
            else
            {
                TempData.append(*it);
                hexData.append(TempData.toInt(&ok, 16));
            }
        }

    }

    return hexData;
}


QString MainWindow::HexToStr(QByteArray receivedata)
{
    QString strData;

    for(auto it = receivedata.begin(); it != receivedata.end();it++)
    {
        if((quint8)*it < 16)
        {
            strData.append('0');
        }
        strData.append(QString().number((quint8)*it,16).toUpper());
        strData.append(' ');

    }

    return strData;
}

