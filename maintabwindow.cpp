#include "maintabwindow.h"
#include "ui_maintabwindow.h"

MainTabWindow::MainTabWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MainTabWindow)
{
    ui->setupUi(this);
}

MainTabWindow::~MainTabWindow()
{
    delete ui;
}

void MainTabWindow::setUsername(const QString& username)
{
    ui->loginLabel->setText(QString("You logged in as %1").arg(username));
}

void MainTabWindow::on_disconnectButton_clicked()
{
    emit logout();
}
