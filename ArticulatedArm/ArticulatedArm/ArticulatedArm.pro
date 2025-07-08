QT       += core gui
QT       += opengl
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ddr6robotwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    robot3dforddr6form.cpp \
    robotcontrolform.cpp \
    robotdialog.cpp \
    rrglwidget.cpp \
    stlfileloader.cpp

HEADERS += \
    ddr6robotwidget.h \
    mainwindow.h \
    robot3dforddr6form.h \
    robotcontrolform.h \
    robotdialog.h \
    rrglwidget.h \
    stlfileloader.h

FORMS += \
    mainwindow.ui \
    robot3dforddr6form.ui \
    robotcontrolform.ui \
    robotdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

win32{
RC_ICONS = res/image/app.ico
RC_LANG = 0x0004
QMAKE_TARGET_COMPANY =
QMAKE_TARGET_PRODUCT = RobotModel
QMAKE_TARGET_DESCRIPTION = RobotModel
QMAKE_TARGET_COPYRIGHT =
}
unix|win32: LIBS += -lOpenGL32
