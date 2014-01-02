#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtSql>
#include <QtWidgets>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_authorizationWidget(new AuthorizationForm())
{
    ui->setupUi(this);
    ui->stackedWidget->addWidget(m_authorizationWidget);
    ui->stackedWidget->setCurrentWidget(m_authorizationWidget);

    connect(m_authorizationWidget, SIGNAL(authorization(AuthorizationForm::AuthData)), SLOT(onAuthorization(AuthorizationForm::AuthData)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Exit_triggered()
{
    QApplication::quit();
}

void MainWindow::onAuthorization(const AuthorizationForm::AuthData& authData)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    QString hostname = QString("Driver={SQL Server};Server=%1;Database=%2;User Id=%3;Password=%4;")
            .arg(authData.hostname)
            .arg("SportInfrastructure")
            .arg(authData.login)
            .arg(authData.password);
    db.setDatabaseName(hostname);

    if (!db.open()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), db.lastError().text());
        db.close();
        return;
    }

}
