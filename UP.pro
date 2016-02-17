#-------------------------------------------------
#
# Project created by QtCreator 2016-02-06T22:53:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ultrapiet
TEMPLATE = app

#if enabled this console option,this application cat work by windows cmd , but the power will down gread...
#if you have mintty, it works naturally !!
#CONFIG += console

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    widgettest.cpp \
    pieteditor.cpp \
    pietcore.cpp \
    piettree.cpp

HEADERS  += mainwindow.h \
    widgettest.h \
    pietcore.h \
    defines.h \
    pieteditor.h \
    piettree.h

FORMS    += mainwindow.ui \
    widgettest.ui

RESOURCES     = qdarkstyle/style.qrc
