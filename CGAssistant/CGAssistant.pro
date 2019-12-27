#-------------------------------------------------
#
# Project created by QtCreator 2017-04-24T16:23:35
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = CGAssistant
TEMPLATE = app
DESTDIR = ../build

DEFINES += CGA_IMPORT

TRANSLATIONS+=lang.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    scriptform.cpp \
    playerform.cpp \
    processform.cpp \
    processtable.cpp \
    autobattleform.cpp \
    player.cpp \
    psworker.cpp \
    battle.cpp \
    itemform.cpp \
    battlesetting.cpp \
    itemdropper.cpp \
    mylistview.cpp \
    itemtweaker.cpp \
    mapform.cpp \
    mypaintmap.cpp \
    accountform.cpp

HEADERS  += mainwindow.h \
    scriptform.h \
    playerform.h \
    processform.h \
    processtable.h \
    autobattleform.h \
    player.h \
    psworker.h \
    battle.h \
    itemform.h \
    itemdropper.h \
    battlesetting.h \
    mylistview.h \
    itemtweaker.h \
    mapform.h \
    mypaintmap.h \
    accountform.h

FORMS    += mainwindow.ui \
    scriptform.ui \
    playerform.ui \
    processform.ui \
    autobattleform.ui \
    itemform.ui \
    mapform.ui \
    accountform.ui

RC_FILE += CGAssistant.rc

INCLUDEPATH += "../boost_1_63_0"
INCLUDEPATH += "../rest_rpc/iguana/third_party/msgpack/include"

DISTFILES +=

RESOURCES += \
    resource.qrc

QMAKE_CXXFLAGS += /MP /Gm-
QMAKE_CFLAGS += /MP /Gm-

Release:LIBS += -L"../Release/" -lCGALib -lntdll
Debug:LIBS += -L"../Debug/" -lCGALib -lntdll
LIBS += -L"../boost_1_63_0/stage/lib"
LIBS += -luser32
LIBS += -lgdi32
