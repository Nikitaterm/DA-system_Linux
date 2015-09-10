#-------------------------------------------------
#
# Project created by QtCreator 2015-07-04T13:53:18
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = GUI_App
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    session.cpp \
    newsessiondialog.cpp \
    plotter.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    session.h \
    messagehandler.h \
    newsessiondialog.h \
    plotter.h

FORMS    += mainwindow.ui \
    newsessiondialog.ui
