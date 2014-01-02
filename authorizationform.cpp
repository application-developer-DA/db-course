#include "authorizationform.h"
#include "ui_authorizationform.h"

AuthorizationForm::AuthorizationForm(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AuthorizationForm)
{
    ui->setupUi(this);
}

AuthorizationForm::~AuthorizationForm()
{
    delete ui;
}

void AuthorizationForm::on_loginButton_clicked()
{
    QString host = ui->hostnameEdit->text();
    QString login = ui->loginEdit->text();
    QString password = ui->passwordEdit->text();

    AuthData authData = { host, login, password };
    emit authorization(authData);
}
