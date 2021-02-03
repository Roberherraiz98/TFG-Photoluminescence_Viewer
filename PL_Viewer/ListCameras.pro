#-------------------------------------------------
#
# Project created by QtCreator 2020-05-12T10:23:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ListCameras
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    ../../../Vimba_3_1/VimbaCPP/Examples/ListCameras/Source/ListCameras.cpp \
    synchronousgrab.cpp \
    ../../../Vimba_3_1/VimbaCPP/Examples/SynchronousGrab/Qt/Source/ApiController.cpp \
    ../../../Vimba_3_1/VimbaCPP/Examples/SynchronousGrab/Qt/Source/CameraObserver.cpp \
    listfeatures.cpp \
    changefeatures.cpp \
    changefeatureswindow.cpp \
    asynchronousgrab.cpp \
    ../../../Vimba_3_1/VimbaCPP/Examples/AsynchronousGrab/Qt/Source/AApiController.cpp \
    ../../../Vimba_3_1/VimbaCPP/Examples/AsynchronousGrab/Qt/Source/FrameObserver.cpp \
    retardtime.cpp \
    pwmduration.cpp

HEADERS += \
        mainwindow.h \
    ../../../Vimba_3_1/VimbaCPP/Examples/ListCameras/Source/ListCameras.h \
    synchronousgrab.h \
    ../../../Vimba_3_1/VimbaCPP/Examples/SynchronousGrab/Qt/Source/ApiController.h \
    ../../../Vimba_3_1/VimbaCPP/Examples/SynchronousGrab/Qt/Source/CameraObserver.h \
    listfeatures.h \
    changefeatures.h \
    changefeatureswindow.h \
    asynchronousgrab.h \
    ../../../Vimba_3_1/VimbaCPP/Examples/AsynchronousGrab/Qt/Source/AApiController.h \
    ../../../Vimba_3_1/VimbaCPP/Examples/AsynchronousGrab/Qt/Source/FrameObserver.h \
    ../../../Vimba_3_1/VimbaImageTransform/Include/VmbCommonTypes.h \
    ../../../Vimba_3_1/VimbaImageTransform/Include/VmbTransform.h \
    ../../../Vimba_3_1/VimbaImageTransform/Include/VmbTransformTypes.h \
    retardtime.h \
    pwmduration.h

INCLUDEPATH += \
        "/home/ubuntu/Vimba_3_1"

DEPENDPATH += \
        "/home/ubuntu/Vimba_3_1"


FORMS += \
        mainwindow.ui \
    synchronousgrab.ui \
    changefeatureswindow.ui \
    asynchronousgrab.ui \
    retardtime.ui \
    pwmduration.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Vimba_3_1/VimbaCPP/DynamicLib/arm_64bit/release/ -lVimbaCPP
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Vimba_3_1/VimbaCPP/DynamicLib/arm_64bit/debug/ -lVimbaCPP
else:unix: LIBS += -L$$PWD/../../../Vimba_3_1/VimbaCPP/DynamicLib/arm_64bit/ -lVimbaCPP

INCLUDEPATH += $$PWD/../../../Vimba_3_1/VimbaCPP/DynamicLib/arm_64bit
DEPENDPATH += $$PWD/../../../Vimba_3_1/VimbaCPP/DynamicLib/arm_64bit

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Vimba_3_1/VimbaCPP/DynamicLib/arm_64bit/release/ -lVimbaC
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Vimba_3_1/VimbaCPP/DynamicLib/arm_64bit/debug/ -lVimbaC
else:unix: LIBS += -L$$PWD/../../../Vimba_3_1/VimbaCPP/DynamicLib/arm_64bit/ -lVimbaC

INCLUDEPATH += $$PWD/../../../Vimba_3_1/VimbaCPP/DynamicLib/arm_64bit
DEPENDPATH += $$PWD/../../../Vimba_3_1/VimbaCPP/DynamicLib/arm_64bit

QMAKE_LFLAGS += -Wl,-rpath,"'\$$ORIGIN'"


INCLUDEPATH += $$PWD/../../../Vimba_3_1/VimbaCPP/DynamicLib/arm_64bit
DEPENDPATH += $$PWD/../../../Vimba_3_1/VimbaCPP/DynamicLib/arm_64bit

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Vimba_3_1/VimbaImageTransform/DynamicLib/arm_64bit/release/ -lVimbaImageTransform
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Vimba_3_1/VimbaImageTransform/DynamicLib/arm_64bit/debug/ -lVimbaImageTransform
else:unix: LIBS += -L$$PWD/../../../Vimba_3_1/VimbaImageTransform/DynamicLib/arm_64bit/ -lVimbaImageTransform

INCLUDEPATH += $$PWD/../../../Vimba_3_1/VimbaImageTransform/DynamicLib/arm_64bit
DEPENDPATH += $$PWD/../../../Vimba_3_1/VimbaImageTransform/DynamicLib/arm_64bit


LIBS += -L/usr/local/lib -lrt -lcrypt -lwiringPi
LIBS += -L/usr/local/lib -lrt -lcrypt -lpthread
