QT += core network
QT -= gui

TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

CONFIG += console
CONFIG -= app_bundle

# Input
SOURCES += miirketclient.cpp clientMain.cpp json.cpp connectorTop.cpp connectorInner.cpp
HEADERS += clientMain.h json.h connectorTop.h connectorInner.h

LIBS += -L/usr/local/lib
