#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class AuthorizationForm;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_Exit_triggered();

private:
    Ui::MainWindow* ui;

    AuthorizationForm* m_authorizationWidget;
};

#endif // MAINWINDOW_H
