QT       += core gui serialport sql charts opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/glm/ \

include($$PWD/glm/glm.pri)

DISTFILES += \
    glm/glm.pri \

SOURCES += \
    DataModel.cpp \
    Scene.cpp \
    Shader.cpp \
    comm.cpp \
    database.cpp \
    historymodel.cpp \
    login.cpp \
    logon.cpp \
    main.cpp \
    mainwidget.cpp \
    model_3d.cpp \
    qua_dispaly.cpp \
    temp_hum_display.cpp


HEADERS += \
    DataModel.h \
    Scene.h \
    Shader.h \
    comm.h \
    database.h \
    historymodel.h \
    login.h \
    logon.h \
    mainwidget.h \
    model_3d.h \
    qua_dispaly.h \
    temp_hum_display.h


FORMS += \
    comm.ui \
    login.ui \
    logon.ui \
    mainwidget.ui \
    qua_dispaly.ui \
    temp_hum_display.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

