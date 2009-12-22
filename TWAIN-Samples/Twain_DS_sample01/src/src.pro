unix { 
    TEMPLATE = lib
    VERSION = 1.0
    DEFINES += UNIX
}
PROJECT_FILE_NAME = TWAINDS_Sample01
TARGET = $$PROJECT_FILE_NAME
LIBS += -lfreeimage

# TWAIN datasources end with a .ds instead of .so
QMAKE_POST_LINK = cp \
    $(TARGET) \
    $${PROJECT_FILE_NAME}.ds
ds.path = /usr/local/lib/twain/sample01
ds.files = $${PROJECT_FILE_NAME}.ds \
    TWAIN_logo.png
INSTALLS += ds
QMAKE_CLEAN = $${PROJECT_FILE_NAME}.ds
INCLUDEPATH = /usr/local/include \
    ../../pub/external/include \
    ../../pub/include \
    ../../common
SOURCES += ../../common/CTiffWriter.cpp \
    ../../common/CommonTWAIN.cpp \
    CommonDS.cpp \
    CScanner_FreeImage.cpp \
    CTWAINDS_Base.cpp \
    CTWAINDS_FreeImage.cpp \
    DSMInterface.cpp \
    TWAINContainer.cpp \
    TWAINContainerFix32.cpp \
    TWAINContainerFrame.cpp \
    TWAINContainerInt.cpp \
    CTWAINDS_Sample1.cpp \
    TWAIN_UI.cpp \
    TWAINContainerString.cpp \
    TWAINContainerFix32Range.cpp \
    TWAINContainerBool.cpp \
    QT_UI.cpp \
    Qt_MainForm.cpp \
    Qt_About.cpp \
    Qt_ProfileName.cpp
HEADERS += ../../common/CTiffWriter.h \
    CScanner_FreeImage.h \
    CTWAINDS_Base.h \
    CTWAINDS_FreeImage.h \
    ../../common/Common.h \
    CommonDS.h \
    ../../common/CommonTWAIN.h \
    DSMInterface.h \
    TWAINContainerFix32.h \
    TWAINContainerFrame.h \
    TWAINContainer.h \
    TWAINContainerInt.h \
    CTWAINDS_Sample1.h \
    ui_MainForm.h \
    ui_About.h \
    TWAIN_UI.h \
    TWAINContainerString.h \
    TWAINContainerFix32Range.h \
    TWAINContainerBool.h \
    QT_UI.h \
    Qt_MainForm.h \
    ui_About.h \
    Qt_About.h \
    Qt_ProfileName.h
CONFIG += warn_on \
    debug

# this project doesn't need any QT libs.
QMAKE_LIBS_QT = 
QMAKE_LIBS_QT_THREAD = 
FORMS += MainForm.ui \
    About.ui \
    ProfileName.ui
