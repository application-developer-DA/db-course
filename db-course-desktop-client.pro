#-------------------------------------------------
#
# Project created by QtCreator 2013-12-22T20:24:19
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = db-course-desktop-client
TEMPLATE = app


SOURCES += main.cpp\
    AuthorizationForm.cpp \
    ClubEditForm.cpp \
    MainTabWindow.cpp \
    MainWindow.cpp \
    PersonEditForm.cpp

HEADERS  += \
    QDateEditSqlDelegate.h \
    AuthorizationForm.h \
    ClubEditForm.h \
    MainTabWindow.h \
    MainWindow.h \
    PersonEditForm.h \
    QCustomDateEdit.h

FORMS    += mainwindow.ui \
    authorizationform.ui \
    maintabwindow.ui

QMAKE_CXXFLAGS += -std=c++11

RESOURCES += \
    res.qrc
