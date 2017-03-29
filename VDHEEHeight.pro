#-------------------------------------------------
#
# Project created by QtCreator 2015-11-16T17:40:04
#
#-------------------------------------------------

QT       += core gui
CONFIG += app_bundle
CONFIG +=  c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VDHEEHeight
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    videoprocess.cpp \
    bfm.cpp \
    tools.cpp \
    display.cpp \
    ffm.cpp \
    dialogrange.cpp \
    qlabelclick.cpp \
    cameracalibrator.cpp \
    settings.cpp \
    calibrater.cpp \
    groundplane.cpp \
    heightprocess.cpp

INCLUDEPATH += C:/opencv/build3conrib/install/include
INCLUDEPATH += "C:/Program Files (x86)/bgs/include"
INCLUDEPATH += E:/QTWS/VDHEEHeight/exLib

include(E:/QTWS/VDHEEHeight/exLib/bgslib.pri)

LIBS += "C:/opencv/build3conrib/bin/*.dll"
LIBS += "C:/Program Files (x86)/bgs/bin/*.dll"

HEADERS  += mainwindow.h \
    bfm.h \
    videoProcess.h \
    tools.h \
    display.h \
    ffm.h \
    dialogrange.h \
    qlabelclick.h \
    settings.h \
    calibrater.h \
    groundplane.h \
    heightprocess.h


FORMS    += mainwindow.ui \
    dialogrange.ui \
    calibratedialog.ui

DISTFILES +=





