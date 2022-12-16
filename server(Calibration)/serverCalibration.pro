#-------------------------------------------------
#
# Project created by QtCreator 2017-03-28T11:48:44
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT += sql
QT += serialport
QT += axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = server
TEMPLATE = app


SOURCES += main.cpp\
        delegate/PersistentStyledItemDelegate.cpp \
        libmodbus/modbus-data.c \
        libmodbus/modbus-rtu.c \
        libmodbus/modbus-tcp.c \
        libmodbus/modbus.c \
        mainwindow.cpp

HEADERS  += mainwindow.h \
    delegate/ButtonDelegate.h \
    delegate/ComboBoxDelegate.h \
    delegate/DateTimeEditDelegate.h \
    delegate/IconDelegate.h \
    delegate/LineEditDelegate.h \
    delegate/PersistentStyledItemDelegate.h \
    delegate/ProgressBarDelegate.h \
    delegate/ReadOnlyDelegate.h \
    delegate/SpinBoxDelegate.h \
    libmodbus/config.h \
    libmodbus/modbus-private.h \
    libmodbus/modbus-rtu-private.h \
    libmodbus/modbus-rtu.h \
    libmodbus/modbus-tcp-private.h \
    libmodbus/modbus-tcp.h \
    libmodbus/modbus-version.h \
    libmodbus/modbus-version.h.in \
    libmodbus/modbus.h
LIBS += -Ldll -lws2_32

FORMS    += mainwindow.ui

TARGET = AGCSimulate

DISTFILES += \
    libmodbus/win32/modbus.dll.manifest.in
