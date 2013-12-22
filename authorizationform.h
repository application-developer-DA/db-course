#ifndef AUTHORIZATIONFORM_H
#define AUTHORIZATIONFORM_H

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

private:
    Ui::AuthorizationForm* ui;
};

#endif // AUTHORIZATIONFORM_H
