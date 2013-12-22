#-------------------------------------------------
#
# Project created by QtCreator 2013-12-22T20:24:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = db-course-desktop-client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    authorizationform.cpp

HEADERS  += mainwindow.h \
    authorizationform.h

FORMS    += mainwindow.ui \
    authorizationform.ui

QMAKE_CXXFLAGS += -std=c++11
