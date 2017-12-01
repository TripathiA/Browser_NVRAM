QT += widgets webkitwidgets
SOURCES = \
    browser.cpp \
    mythread.cpp
HEADERS += \
    browser.h \
    mythread.h
QMAKE_CFLAGS += -std=c++11 -fpermissive
QMAKE_CXXFLAGS += -std=c++11 -fpermissive
QMAKE_LFLAGS += -std=c++11 -fpermissive

DISTFILES += \
    aastha.txt
