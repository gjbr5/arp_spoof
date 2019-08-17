TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lpcap
LIBS += -pthread

SOURCES += \
        hostinfo.cpp \
        hwaddress.cpp \
        inetaddress.cpp \
        main.cpp \
        packetctrl.cpp \
        packetheader.cpp \
        session.cpp

HEADERS += \
    hostinfo.h \
    hwaddress.h \
    inetaddress.h \
    packetctrl.h \
    packetheader.h \
    session.h
