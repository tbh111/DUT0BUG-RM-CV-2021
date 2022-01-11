QT += core gui
QT += widgets
QT += serialbus
CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

message("infantry v0.1")

SOURCES += \
        src/anglesolver.cpp \
        src/commprotocol.cpp \
#        src/energydetector.cpp \
        src/imageshow.cpp \
        src/config.cpp \
        src/main.cpp \
        src/taskmanager.cpp \
        src/processthread.cpp \
        src/capturethread.cpp \
        src/armordetector.cpp

HEADERS += \
        include/anglesolver.h \
        include/common.h \
        include/commprotocol.h \
#        include/energydetector.h \
        include/imageshow.h \
        include/config.h \
        include/CameraApi.h \
        include/CameraDefine.h \
        include/CameraStatus.h \
        include/taskmanager.h \
        include/processthread.h \
        include/capturethread.h \
        include/armordetector.h

COMPILER = $$system($$QMAKE_CC -v 2>&1)
contains(COMPILER, x86_64-linux-gnu){
    message("compile on linux, x86_64-linux-gnu")
    LIBS += /home/tbh/DUT0BUG-RM-CV-2021/lib/x64/libMVSDK.so
}
contains(COMPILER, aarch64-linux-gnu){
    message("compile on linux, aarch64-linux-gnu")
    LIBS += /home/dji/DUT0BUG-RM-CV-2021/lib/arm64/libMVSDK.so
}

INCLUDEPATH += /usr/local/include/opencv4/
INCLUDEPATH += /usr/local/include/eigen3
INCLUDEPATH += ./include/
LIBS += /usr/local/lib/libopencv_*.so

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    imageshow.ui
