#include "ui_AuthorizationForm.h"

#include "AuthorizationForm.h"

AuthorizationForm::AuthorizationForm(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AuthorizationForm)
{
    ui->setupUi(this);
    ui->hostnameEdit->setText("Daniel-PC\\SQLEXPRESS");
}

AuthorizationForm::~AuthorizationForm()
{
    delete ui;
}

void AuthorizationForm::resetAuthData()
{
    ui->hostnameEdit->setText("");
    ui->loginEdit->setText("");
    ui->passwordEdit->setText("");
}

void AuthorizationForm::on_loginButton_clicked()
{
    QString host = ui->hostnameEdit->text();
    QString login = ui->loginEdit->text();
    QString password = ui->passwordEdit->text();

    AuthData authData = { host, login, password };
    emit authorization(authData);
}
