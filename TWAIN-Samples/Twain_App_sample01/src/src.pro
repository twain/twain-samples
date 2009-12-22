unix { 
    TEMPLATE = app
    VERSION = 1.0
    DEFINES += UNIX \
        kTWAIN_DS_DIR=\"/usr/local/lib/twain\"
}
TARGET = twain_app_sample01
LIBS += -lfreeimage
INCLUDEPATH = /usr/local/include \
    ../../pub/external/include \
    ../../pub/include \
    ../../common
SOURCES += ../../common/CTiffWriter.cpp \
    ../../common/CommonTWAIN.cpp \
    DSMInterface.cpp \
    main.cpp \
    TwainApp.cpp \
    TwainApp_ui.cpp \
    ../../common/TwainString.cpp \
    TwainAppCMD.cpp
HEADERS += ../../common/Common.h \
    ../../common/CTiffWriter.h \
    ../../common/CommonTWAIN.h \
    DSMInterface.h \
    TwainApp.h \
    TwainApp_ui.h
CONFIG += warn_off \
    debug

# this project doesn't need any QT libs.
QMAKE_LIBS_QT = 
QMAKE_LIBS_QT_THREAD = 
