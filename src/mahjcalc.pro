QT += widgets

HEADERS       = button.h \
                calculator.h \
    gamestate.h
SOURCES       = button.cpp \
                calculator.cpp \
                main.cpp \
    gamestate.cpp

QMAKE_CXXFLAGS += -std=c++11

# install
target.path = C:/Qt/Projects/calculator
INSTALLS += target
