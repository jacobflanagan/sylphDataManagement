#-------------------------------------------------
#
# Project created by QtCreator 2019-02-08T16:36:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
@
QT += quick
QT += quickwidgets qml location positioning xml
@

TARGET = SylphDataManagement
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

CONFIG += c++19

SOURCES += \
        main.cpp \
    mainwindow.cpp \
    projectwidget.cpp \
    missionwidget.cpp \
    p4dproject.cpp \
    maptools.cpp \
    exif.cpp \
    newprojectdialog.cpp \
    newmissiondialog.cpp \
    igisproject.cpp

HEADERS += \
        mainwindow.h \
    projectwidget.h \
    missionwidget.h \
    p4dproject.h \
    maptools.h \
    exif.h \
    newprojectdialog.h \
    newmissiondialog.h \
    igisproject.h

FORMS += \
        mainwindow.ui \
    projectwidget.ui \
    missionwidget.ui \
    newprojectdialog.ui \
    newmissiondialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    map2.qml \
    IGIS.png \
    deleteme.qml \
    map3d.qml \
    map1.qml \
    map.qml \
    polygon.qml \
    circle.qml \
    mapview.qml \
    marker.qml \
    polygon.qml \
    rectangle.qml \
    mapviewer.qml \
    map-marker-icon.png \
    mapview2.qml

unix|win32: LIBS += -L$$PWD/../build-libexif-Desktop_Qt_5_12_1_MinGW_64_bit-Debug/debug/ -llibexif

INCLUDEPATH += $$PWD/../libexif/
DEPENDPATH += $$PWD/../libexif/

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../build-libexif-Desktop_Qt_5_12_1_MinGW_64_bit-Debug/debug/libexif.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../build-libexif-Desktop_Qt_5_12_1_MinGW_64_bit-Debug/debug/liblibexif.a

