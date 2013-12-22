#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "authorizationform.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_authorizationWidget(new AuthorizationForm())
{
    ui->setupUi(this);
    ui->stackedWidget->addWidget(m_authorizationWidget);
    ui->stackedWidget->setCurrentWidget(m_authorizationWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Exit_triggered()
{
    QApplication::quit();
}
