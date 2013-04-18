############################################################
#                                                          #
#      Part of the OneBible Project for BlackBerry 10      #
# Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013 #
#                                                          #
#                   All rights reserved.                   #
#                                                          #
############################################################

APP_NAME = youversion-bb10
CONFIG += qt warn_on cascades10
QT += network gui
#QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O2
LIBS += -lbbplatform -lbbsystem -lbps -lscreen -lmmrndclient -lstrm -lbbdata
include(config.pri)
