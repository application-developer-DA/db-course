#pragma once

#include <QWidget>
#include <QLabel>
#include <QMovie>
#include <QVBoxLayout>

class WaitingWnd : public QWidget
{
    Q_OBJECT

public:
    WaitingWnd(QWidget* parent = nullptr) : QWidget(parent), animation(":res/res/loading_animation.gif")
    {
        QLabel* animationLabel = new QLabel(this);
        animationLabel->setMovie(&animation);

        QLabel* textLabel = new QLabel("Loading, please wait...", this);

        QVBoxLayout* layout = new QVBoxLayout;
        layout->addWidget(animationLabel);
        layout->addWidget(textLabel);

        setLayout(layout);
        animation.start();
    }

private:
    QMovie animation;
};
