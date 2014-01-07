#pragma once

#include <QWidget>
#include <QLabel>
#include <QMovie>
#include <QGridLayout>

class WaitingWnd : public QWidget
{
    Q_OBJECT

public:
    WaitingWnd(QWidget* parent = nullptr) : QWidget(parent), animation(":res/res/loading_animation.gif")
    {
        QLabel* animationLabel = new QLabel(this);
        animationLabel->setMovie(&animation);

        QLabel* textLabel = new QLabel("Loading, please wait...", this);

        QGridLayout* layout = new QGridLayout;
        layout->addWidget(animationLabel);
        layout->addWidget(textLabel);

        setLayout(layout);
        animation.start();
    }

private:
    QMovie animation;
};
