#-------------------------------------------------
#
# Project created by QtCreator 2024-03-06T09:17:29
#
#-------------------------------------------------

QT       += core gui
QT       += xml
QT       += svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QGisdemo
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
INCLUDEPATH += /usr/include/qgis
LIBS += -L/usr/lib -lqgis_core -lqgis_gui
INCLUDEPATH += /usr/include/gdal
LIBS += -lgdal
SOURCES += \
        main.cpp \
        qgisdemo.cpp \
    qgsgraphmap.cpp \
    myfeaturetool.cpp \
    mygdaltool.cpp

HEADERS += \
        qgisdemo.h \
    qgsgraphmap.h \
    data.h \
    myfeaturetool.h \
    mygdaltool.h

FORMS += \
        qgisdemo.ui
