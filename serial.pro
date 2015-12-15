#-------------------------------------------------
#
# Project created by QtCreator 2015-12-05T16:50:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serial
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    helpwindow.cpp

HEADERS  += mainwindow.h \
    helpwindow.h

FORMS    += mainwindow.ui \
    helpwindow.ui
QT	 += serialport
