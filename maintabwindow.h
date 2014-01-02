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

private:
    Ui::MainTabWindow* ui;
};

