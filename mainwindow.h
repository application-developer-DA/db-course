#pragma once

#include "authorizationform.h"

#include <QMainWindow>

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

    void onAuthorization(const AuthorizationForm::AuthData& authData);

private:
    Ui::MainWindow* ui;

    AuthorizationForm* m_authorizationWidget;
};
