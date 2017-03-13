QT += core
QT -= gui

CONFIG += c++11

TARGET = ANMS-Codes
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += /usr/include/opencv
LIBS += -lopencv_core
LIBS += -lopencv_highgui
LIBS += -lopencv_features2d

SOURCES += main.cpp

HEADERS += \
    nanoflann.hpp \
    range-tree/lrtypes.h \
    range-tree/ranget.h \
    anms.h
