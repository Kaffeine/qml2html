QT = core qml

TARGET = qml2html
CONFIG += c++11 console
CONFIG -= app_bundle

SOURCES += main.cpp
RESOURCES += qml.qrc

OTHER_FILES += index.qml
