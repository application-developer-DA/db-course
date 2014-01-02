#pragma once

#include <QWidget>

namespace Ui {
class MainTabWindow;
}

class MainTabWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainTabWindow(QWidget* parent = nullptr);
    ~MainTabWindow();

    void setUsername(const QString& username);

signals:
    void logout();

private slots:
    void on_disconnectButton_clicked();

private:
    Ui::MainTabWindow* ui;
};

