#-------------------------------------------------
#
# Project created by QtCreator 2014-06-18T16:36:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fileEncrypt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    encrypt.c \
    md5.c \
    pwddialog.cpp

HEADERS  += mainwindow.h \
    encrypt.h \
    md5.h \
    pwddialog.h

FORMS    += mainwindow.ui \
    pwddialog.ui

RESOURCES += \
    icon.qrc
