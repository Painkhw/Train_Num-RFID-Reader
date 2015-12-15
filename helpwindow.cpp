#include "helpwindow.h"
#include "ui_helpwindow.h"

HelpWindow::HelpWindow(QWidget *parent) :
    QWidget(parent),
    help_ui(new Ui::HelpWindow)
{
    help_ui->setupUi(this);
    this->setWindowTitle(tr("About"));
}

HelpWindow::~HelpWindow()
{
    delete help_ui;
}

void HelpWindow::on_pushButton_clicked()
{
    this->close();
}
