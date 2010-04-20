unix { 
    TEMPLATE = lib
    VERSION = 1.0
    DEFINES += UNIX
}
PROJECT_FILE_NAME = TWAINDS_FreeImage
TARGET = $$PROJECT_FILE_NAME
LIBS += -lfreeimage

# TWAIN datasources end with a .ds instead of .so
QMAKE_POST_LINK = cp \
    $(TARGET) \
    $${PROJECT_FILE_NAME}.ds
ds.path = /usr/local/lib/twain/FreeImage
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
HEADERS += ../../common/Common.h \
    ../../common/CommonTWAIN.h \
    ../../common/CTiffWriter.h \
    CScanner_FreeImage.h \
    CTWAINDS_Base.h \
    CTWAINDS_FreeImage.h \
    CommonDS.h \
    DSMInterface.h \
    TWAINContainerFix32.h \
    TWAINContainerFrame.h \
    TWAINContainer.h \
    TWAINContainerInt.h \
    CTWAINDS_Sample1.h \
    TWAIN_UI.h \
    TWAINContainerString.h \
    TWAINContainerFix32Range.h \
    TWAINContainerBool.h \
    QT_UI.h \
    Qt_MainForm.h \
    Qt_About.h \
    Qt_ProfileName.h \
    ui_About.h \
    ui_MainForm.h \
    ui_About.h
TEMPLATE = lib
CONFIG += warn_on \
    debug \
    x86 ppc

QMAKE_BUNDLE_EXTENSION = ds
# this project doesn't need any QT libs.
QMAKE_LIBS_QT = 
QMAKE_LIBS_QT_THREAD = 
FORMS += MainForm.ui \
    About.ui \
    ProfileName.ui
RESOURCES += qtresources.qrc
