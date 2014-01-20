#include "ui_mainwindow.h"

#include "MainWindow.h"

#include <QtWidgets>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_authorizationWindow(new AuthorizationForm())
    , m_tabWindow(new MainTabWindow())
    , m_waitingWnd(new WaitingWnd())
    , m_db(QSqlDatabase::addDatabase("QODBC"))
{
    ui->setupUi(this);
    ui->stackedWidget->addWidget(m_authorizationWindow);
    ui->stackedWidget->addWidget(m_tabWindow);
    ui->stackedWidget->addWidget(m_waitingWnd);
    ui->stackedWidget->setCurrentWidget(m_authorizationWindow);

    connect(m_authorizationWindow, SIGNAL(authorization(AuthorizationForm::AuthData)), SLOT(onLogin(AuthorizationForm::AuthData)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onLogin(const AuthorizationForm::AuthData& authData)
{
    qDebug() << authData.hostname << authData.username << authData.password;

    // showWaitingWnd(true);
    // qApp->processEvents();

    QString hostname = QString("Driver={SQL Server};Server=%1;Database=%2;")
            .arg(authData.hostname)
            .arg("SportInfrastructure");
    m_db.setConnectOptions();
    m_db.setUserName(authData.username);
    m_db.setPassword(authData.password);
    m_db.setDatabaseName(hostname);

    if (!m_db.open()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), m_db.lastError().text());
        m_db.close();
        // showWaitingWnd(false);
        return;
    }

    m_tabWindow->login();
    ui->stackedWidget->setCurrentWidget(m_tabWindow);
}

void MainWindow::on_action_Exit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionLogout_triggered()
{
    if (m_db.isOpen())
        m_db.close();
    m_authorizationWindow->resetAuthData();
    ui->stackedWidget->setCurrentWidget(m_authorizationWindow);
}

void MainWindow::showWaitingWnd(bool show)
{
    if (show)
        ui->stackedWidget->setCurrentWidget(m_waitingWnd);
    else
        ui->stackedWidget->setCurrentWidget(m_authorizationWindow);
}
