#-------------------------------------------------
#
# Project created by QtCreator 2017-06-11T14:07:48
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QTD
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    Dialog/logindialog.cpp \
    core.cpp \
    Dialog/measurementdialog.cpp \
    Dialog/monitordialog.cpp \
    Dialog/newmeasurementdialog.cpp \
    Dialog/parametersdialog.cpp \
    Dialog/seriessourcesdialog.cpp \
    Dialog/settingsdialog.cpp \
    Service/Analyzer/analyzer.cpp \
    Service/Analyzer/bad1.cpp \
    Service/DB/database.cpp \
    Service/DB/tabdb.cpp \
    Service/HV/n1470hv.cpp \
    Service/HV/powersupplyhv.cpp \
    Service/Report/report.cpp \
    Service/measurementworker.cpp \
    Service/rack.cpp \
    Dialog/newuserdialog.cpp \
    Dialog/changepassworddialog.cpp \
    Dialog/authorizationdialog.cpp \
    Service/monitorworker.cpp \
    Service/Analyzer/bad2.cpp

HEADERS  += mainwindow.h \
    Dialog/logindialog.h \
    global.h \
    core.h \
    Dialog/measurementdialog.h \
    Dialog/monitordialog.h \
    Dialog/newmeasurementdialog.h \
    Dialog/parametersdialog.h \
    Dialog/seriessourcesdialog.h \
    Dialog/settingsdialog.h \
    Service/Analyzer/analyzer.h \
    Service/Analyzer/bad1.h \
    Service/DB/database.h \
    Service/DB/tabdb.h \
    Service/HV/n1470hv.h \
    Service/HV/powersupplyhv.h \
    Service/Report/report.h \
    Service/Analyzer/FTDI/ftd2xx.h \
    Service/measurementworker.h \
    Service/rack.h \
    Dialog/newuserdialog.h \
    Dialog/changepassworddialog.h \
    Dialog/authorizationdialog.h \
    Service/monitorworker.h \
    Service/Analyzer/bad2.h

FORMS    += mainwindow.ui \
    Dialog/logindialog.ui \
    Dialog/measurementdialog.ui \
    Dialog/monitordialog.ui \
    Dialog/newmeasurementdialog.ui \
    Dialog/parametersdialog.ui \
    Dialog/seriessourcesdialog.ui \
    Dialog/settingsdialog.ui \
    Dialog/newuserdialog.ui \
    Dialog/changepassworddialog.ui \
    Dialog/authorizationdialog.ui

TRANSLATIONS += $$PWD/Translations/qtd_pl.ts \
                $$PWD/Translations/qtd_en.ts \
                $$PWD/Translations/qtd_sk.ts

CODECFORTR = UTF-8

RESOURCES += \
    qtd.qrc
RC_FILE += \
    icon.rc
win32: LIBS += -L$$PWD/Service/HV/N1470_5.61/Win/lib/x86/ -lCAENHVWrapper

win32: INCLUDEPATH += $$PWD/Service/HV/N1470_5.61/Win/include
win32: DEPENDPATH += $$PWD/Service/HV/N1470_5.61/Win/include

unix: DEFINES = UNIX
unix: LIBS += -L$$PWD/Service/HV/N1470_5.61/Linux/lib/ -lCAENHVWrapper
unix: INCLUDEPATH += $$PWD/Service/HV/N1470_5.61/Linux/include
unix: DEPENDPATH += $$PWD/Service/HV/N1470_5.61/Linux/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Service/Analyzer/FTDI/i386/ -lftd2xx
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Service/Analyzer/FTDI/i386/ -lftd2xx

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/Service/Analyzer/FTDI/i386/ftd2xx.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/Service/Analyzer/FTDI/i386/ftd2xx.lib

INCLUDEPATH += $$PWD/Service/Analyzer/FTDI/i386
DEPENDPATH += $$PWD/Service/Analyzer/FTDI/i386
