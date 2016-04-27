#-------------------------------------------------
#
# Project created by QtCreator 2016-02-06T22:53:43
#
#-------------------------------------------------

QT       += core gui opengl multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



TARGET = ultrapiet
TEMPLATE = app

CONFIG += opengl

QMAKE_CXXFLAGS += -std=c++14

SOURCES += main.cpp\
        mainwindow.cpp \
    pieteditor.cpp \
    pietcore.cpp \
    piettree.cpp \
    executingpietlibraries.cpp \
    loadpietdll.cpp \
    glgamewidget.cpp

HEADERS  += mainwindow.h \
    pietcore.h \
    defines.h \
    pieteditor.h \
    piettree.h \
    executingpietlibraries.h \
    loadpietdll.h \
    glgamewidget.h \
    integerrange.h

FORMS    += mainwindow.ui

RESOURCES     = qdarkstyle/style.qrc


#if Use OpenGL
LIBS += -lopengl32

#for mobile
#CONFIG += mobility
#MOBILITY =


#if enabled this console option,this application cat work by windows cmd , but the power will down gread...
#if you have mintty, it works naturally !!
#CONFIG += console
