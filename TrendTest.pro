#-------------------------------------------------
#
# Project created by QtCreator 2015-04-02T14:39:15
#
#-------------------------------------------------

QT       += core gui

INCLUDEPATH += /usr/include/tango

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = TrendTest
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    qcustomplot.cpp \
    qtrendlegendwidget.cpp \
    attributeselectform.cpp \
    trenddatacollector.cpp

HEADERS  += dialog.h \
    qcustomplot.h \
    qtrendlegendwidget.h \
    attributeselectform.h \
    trenddatacollector.h

FORMS    += dialog.ui \
    attributeselectform.ui

QMAKE_CXXFLAGS += -std=c++0x

unix:!macx: LIBS += -L$$PWD/../../../../usr/lib/x86_64-linux-gnu/ -ltango

INCLUDEPATH += $$PWD/../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../usr/lib/x86_64-linux-gnu

unix:!macx: LIBS += -L$$PWD/../../../../usr/lib/ -lomniORB4
unix:!macx: LIBS += -L$$PWD/../../../../usr/lib/ -lomniDynamic4
unix:!macx: LIBS += -L$$PWD/../../../../usr/lib/ -lomnithread

INCLUDEPATH += $$PWD/../../../../usr/include
DEPENDPATH += $$PWD/../../../../usr/include
