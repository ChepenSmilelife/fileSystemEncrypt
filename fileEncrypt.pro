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
    md5.c \
    pwddialog.cpp \
    xorencrypt.c \
    logindialog.cpp

HEADERS  += mainwindow.h \
    md5.h \
    pwddialog.h \
    qencrypt.h \
    xorencrypt.h \
    logindialog.h

FORMS    += mainwindow.ui \
    pwddialog.ui \
    logindialog.ui

RESOURCES += \
    icon.qrc
