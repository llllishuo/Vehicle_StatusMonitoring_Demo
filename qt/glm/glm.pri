# in root pro file set the CONFIG
# CONFIG += C++17

INCLUDEPATH += $$PWD/

DEFINES += _CRT_SECURE_NO_WARNINGS

HEADERS += $$files($$PWD/glm/detail/*.hpp, true) \
           $$files($$PWD/glm/gtc/*.hpp, true) \
           $$files($$PWD/glm/gtx/*.hpp, true) \
           $$files($$PWD/glm/*.hpp, true)


SOURCES += $$PWD/glm/detail/glm.cpp


DISTFILES += \
    $$PWD/glm/CMakeLists.txt
