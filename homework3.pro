TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        policy.cpp \
        request.cpp \
        segment.cpp \
        stats.cpp

HEADERS += \
    policy.h \
    request.h \
    segment.h \
    stats.h
