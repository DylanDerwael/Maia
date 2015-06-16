#-------------------------------------------------
#
# Project created by QtCreator 2015-04-28T08:49:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
PKGCONFIG += opencv
CONFIG += c++11
LIBS += -ltbb

TARGET = Maia
TEMPLATE = app

SOURCES += main.cpp\
        MainWindow.cpp\
        cameradevice.cpp\
        ConvertMatToGui.cpp

HEADERS  += MainWindow.h\
            cameradevice.h\
            ConvertMatToGui.h \
            Structures.h

FORMS    += MainWindow.ui
