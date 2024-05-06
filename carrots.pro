QT       += core gui
QT += multimedia
QT += multimediawidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    choosewindow.cpp \
    hp.cpp \
    lose.cpp \
    main.cpp \
    mainwindow.cpp \
    menuwindow.cpp \
    money.cpp \
    myobject.cpp \
    scene1.cpp \
    scene2.cpp \
    scene3.cpp \
    tower.cpp \
    win.cpp

HEADERS += \
    choosewindow.h \
    hp.h \
    lose.h \
    mainwindow.h \
    menuwindow.h \
    money.h \
    myobject.h \
    scene1.h \
    scene2.h \
    scene3.h \
    tower.h \
    win.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image.qrc
