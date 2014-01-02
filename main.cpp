#include "mainwindow.h"

#include <QApplication>

#include <QStyleFactory>

#define FUSION_STYLE

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef FUSION_STYLE
    QApplication::setStyle(QStyleFactory::create("Fusion"));
#endif
#ifdef FUSION_DARK_STYLE
    QPalette p;
    p = qApp->palette();
    p.setColor(QPalette::Window, QColor(53,53,53));
    p.setColor(QPalette::Button, QColor(53,53,53));
    p.setColor(QPalette::Highlight, QColor(142,45,197));
    p.setColor(QPalette::ButtonText, QColor(255,255,255));
    qApp->setPalette(p);
#endif

    MainWindow w;
    w.show();

    return a.exec();
}
