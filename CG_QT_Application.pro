#-------------------------------------------------
#
# Project created by QtCreator 2017-10-09T22:09:42
#
#-------------------------------------------------

QT      += core gui
CONFIG  += c++11

win32:QT      += opengl
win32:LIBS += libopengl32
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CG_QT_Application
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    line2d.cpp \
    point2d.cpp \
    float_math.cpp \
    scene.cpp \
    renderer.cpp \
    morphingpanel.cpp \
    createfractaldialog.cpp \
    fractalglwidget.cpp \
    enterthematrixdialog.cpp

HEADERS  += mainwindow.h \
    line2d.h \
    point2d.h \
    float_math.h \
    scene.h \
    renderer.h \
    types.h \
    morphingpanel.h \
    createfractaldialog.h \
    fractalglwidget.h \
    enterthematrixdialog.h

FORMS    += mainwindow.ui \
    createfractaldialog.ui \
    enterthematrixdialog.ui

RESOURCES += \
    shaders.qrc \
    icons.qrc
