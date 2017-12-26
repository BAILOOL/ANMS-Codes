QT += core
QT -= gui

CONFIG += c++11

TARGET = ANMS-Codes
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += /usr/local/include/opencv

LIBS += -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_imgcodecs

SOURCES += main.cpp

HEADERS += \
    nanoflann.hpp \
    range-tree/lrtypes.h \
    range-tree/ranget.h \
    anms.h
