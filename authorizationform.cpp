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
