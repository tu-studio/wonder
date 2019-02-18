##########################################################################################
#                                                                                        #
#      ### Build xwonder on a Windows platform using MSYS2 inside a MinGW Bash ###       #
#                                                                                        #
#                                                                                        #
# 1. Open a MSYS MinGW bash and go to the xwonder folder (e.g. C:\wonder\src\xwonder):   #
# $ cd /c/wonder/src/xwonder                                                             #
#                                                                                        #
# 2. Execute qmake from here (e.g. static Qt5 version of MSYS2):                         #
# $ /c/MSYS64/mingw64/qt5-static/bin/qmake.exe -spec win32-g++ ./xwonder.pro             #
#                                                                                        #
# 3. A Makefile was created by qmake to build xwonder via "make", so just type make:     #
# $ make                                                                                 #
#                                                                                        #
# The resulting executable "xwonder.exe" is located in the "release" subfolder.          #
#                                                                                        #
#                                                                                        #
# General qmake usage on different platforms:                                            #
#                                                                                        #
# Linux  : qmake xwonder.pro                                                             #
# Windows: qmake -spec win32-g++ xwonder.pro                                             #
# MacOS X: qmake -spec macx-g++ xwonder.pro                                              #
#                                                                                        #
##########################################################################################



##################################################################################
# THIS .PRO FILE BUILDS THE RELEASE VERSION OF XWONDER ON LINUX, WINDOWS AND MAC #
##################################################################################



TEMPLATE = app
TARGET = xwonder
CONFIG += qt
CONFIG += release
QT += widgets
QT += opengl
QT += xml

INCLUDEPATH += .
INCLUDEPATH += ../include

DEPENDPATH += .
DEPENDPATH += ../include
DEPENDPATH += ../lib


# Linux stuff
unix:!macx {
    CONFIG += link_pkgconfig
    PKGCONFIG += liblo
    PKGCONFIG += glu
}


# Windows stuff
win32 {
    LIBS += -llo -lws2_32 -liphlpapi
    QMAKE_LFLAGS += -static
    DEFINES += "NDEBUG"
    DEFINES += "WIN32"
    DEFINES += "_WIN32_WINNT=0x601"
    DEFINES += "_USE_MATH_DEFINES"
    RC_ICONS = xwonder.ico
}


# MacOS stuff
macx: {
    CONFIG += link_pkgconfig
    PKGCONFIG += liblo
    # PKGCONFIG += freeglut # or glu?
    LIBS += -framework ApplicationServices
    QMAKE_LFLAGS += -L/System/Library/Frameworks/ApplicationServices.framework/Headers
    INCLUDEPATH += /System/Library/Frameworks/ApplicationServices.framework/Headers
    ICON = xwonder.icns
}


# Header, source and resource files
HEADERS += ../include/oscin.h \
           AddChannelsDialog.h \
           ChannelsWidget.h \
           Colors.h \
           FilenameDialog.h \
           OSCReceiver.h \
           SnapshotNameDialog.h \
           SnapshotSelector.h \
           SnapshotSelectorButton.h \
           Source.h \
           SourceCoordinates.h \
           SourceGroup.h \
           SourcePositionDialog.h \
           Sources3DWidget.h \
           SourceWidget.h \
           StreamClientWidget.h \
           TimeLCDNumber.h \
           XwonderConfig.h \
           XwonderMainWindow.h

SOURCES += ../lib/oscin.cpp \
           AddChannelsDialog.cpp \
           ChannelsWidget.cpp \
           FilenameDialog.cpp \
           OSCReceiver.cpp \
           SnapshotNameDialog.cpp \
           SnapshotSelector.cpp \
           SnapshotSelectorButton.cpp \
           Source.cpp \
           SourceGroup.cpp \
           SourcePositionDialog.cpp \
           Sources3DWidget.cpp \
           SourceWidget.cpp \
           StreamClientWidget.cpp \
           TimeLCDNumber.cpp \
           Xwonder.cpp \
           XwonderConfig.cpp \
           XwonderMainWindow.cpp

RESOURCES += icons.qrc
