QT += core network sql
QT -= gui

TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

CONFIG += console
CONFIG -= app_bundle

# Input
SOURCES += miirketserver.cpp serverMain.cpp json.cpp relayTop.cpp relayInner.cpp sslserver.cpp httpRelayTop.cpp httpRelayInner.cpp
HEADERS += serverMain.h json.h relayTop.h relayInner.h sslserver.h httpRelayTop.h httpRelayInner.h

LIBS += -L/usr/local/lib
