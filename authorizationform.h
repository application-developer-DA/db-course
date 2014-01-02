#pragma once

#include <QWidget>

namespace Ui {
class AuthorizationForm;
}

class AuthorizationForm : public QWidget
{
    Q_OBJECT
public:
    explicit AuthorizationForm(QWidget* parent = nullptr);
    ~AuthorizationForm();

    void resetAuthData();

    struct AuthData {
        QString hostname;
        QString login;
        QString password;
    };

signals:
    void authorization(const AuthorizationForm::AuthData& authData);

private slots:
    void on_loginButton_clicked();

private:
    Ui::AuthorizationForm* ui;
};

