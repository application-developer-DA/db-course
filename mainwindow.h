#pragma once

#include "authorizationform.h"
#include "maintabwindow.h"

#include <QMainWindow>
#include <QtSql>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_Exit_triggered();
    void on_actionLogout_triggered();

    void onLogin(const AuthorizationForm::AuthData& authData);

private:
    Ui::MainWindow* ui;

    AuthorizationForm*  m_authorizationWindow;
    MainTabWindow*      m_tabWindow;

    QSqlDatabase        m_db;
};
