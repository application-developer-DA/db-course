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
