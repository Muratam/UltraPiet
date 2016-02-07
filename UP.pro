#-------------------------------------------------
#
# Project created by QtCreator 2016-02-06T22:53:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UP
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    widgettest.cpp \
    iconeditor.cpp

HEADERS  += mainwindow.h \
    widgettest.h \
    iconeditor.h

FORMS    += mainwindow.ui \
    widgettest.ui

RESOURCES     = qdarkstyle/style.qrc
