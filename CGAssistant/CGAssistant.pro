#-------------------------------------------------
#
# Project created by QtCreator 2017-04-24T16:23:35
#
#-------------------------------------------------

QT       += core gui widgets sql network

TARGET = CGAssistant
TEMPLATE = app

DEFINES += CGA_IMPORT

TRANSLATIONS += lang.ts

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
    accountform.cpp \
    chatform.cpp

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
    accountform.h \
    chatform.h

FORMS    += mainwindow.ui \
    scriptform.ui \
    playerform.ui \
    processform.ui \
    autobattleform.ui \
    itemform.ui \
    mapform.ui \
    accountform.ui \
    chatform.ui \
    chatform.ui

RC_FILE += CGAssistant.rc

INCLUDEPATH += "../qhttp/src"
INCLUDEPATH += "../boost"
INCLUDEPATH += "../rest_rpc/iguana/third_party/msgpack/include"

DISTFILES +=

RESOURCES += resource.qrc

QMAKE_CXXFLAGS += /MP /Gm-
QMAKE_CFLAGS += /MP /Gm-

Release:LIBS += -L"../Release/"
Debug:LIBS += -L"../Debug/"
LIBS += -L"../boost/stage/lib"
LIBS += -L"../qhttp/xbin"
LIBS += -luser32
LIBS += -lgdi32
LIBS += -lntdll
LIBS += -lqhttp
LIBS += -lCGALib

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
