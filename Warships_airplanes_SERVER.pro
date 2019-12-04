#-------------------------------------------------
#
# Project created by QtCreator 2019-10-07T21:37:09
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Warships_airplanes_SERVER
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        gameview.cpp \
        main.cpp \
        mainwindow.cpp \
        server.cpp \
        serverthread.cpp

HEADERS += \
        gameview.h \
        mainwindow.h \
        server.h \
        serverthread.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-Warships_airplanes_DLL-Desktop_Qt_5_9_8_MinGW_32bit-Debug/release/ -lWarships_airplanes_DLL
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-Warships_airplanes_DLL-Desktop_Qt_5_9_8_MinGW_32bit-Debug/debug/ -lWarships_airplanes_DLL
else:unix: LIBS += -L$$PWD/../build-Warships_airplanes_DLL-Desktop_Qt_5_9_8_MinGW_32bit-Debug/ -lWarships_airplanes_DLL

INCLUDEPATH += $$PWD/../airplanes_lib
DEPENDPATH += $$PWD/../airplanes_lib
