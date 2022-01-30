QT += core
QT -= gui

CONFIG += c++11

TARGET = ANMS-Codes
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += /usr/local/include/opencv ../include/

LIBS += -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_imgcodecs

SOURCES += main.cpp

HEADERS += \
    ../include/nanoflann.hpp \
    ../include/range-tree/lrtypes.h \
    ../include/range-tree/ranget.h \
    ../include/anms.h
