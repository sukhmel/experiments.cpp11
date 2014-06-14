QT += widgets

RC_FILE = icon.rc

HEADERS       = button.h \
                calculator.h \
                gamestate.h \
                iohelper.h
SOURCES       = button.cpp \
                calculator.cpp \
                main.cpp \
                gamestate.cpp

QMAKE_CXXFLAGS += -std=c++11

# install
target.path = C:/Qt/Projects/calculator
INSTALLS += target

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml \
    icon.rc
