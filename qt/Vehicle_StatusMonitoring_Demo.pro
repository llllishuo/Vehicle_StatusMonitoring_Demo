QT       += core gui serialport sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DataModel.cpp \
    comm.cpp \
    database.cpp \
    display.cpp \
    historymodel.cpp \
    login.cpp \
    logon.cpp \
    main.cpp \
    mainwidget.cpp

HEADERS += \
    DataModel.h \
    comm.h \
    database.h \
    display.h \
    historymodel.h \
    login.h \
    logon.h \
    mainwidget.h

FORMS += \
    comm.ui \
    display.ui \
    login.ui \
    logon.ui \
    mainwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
