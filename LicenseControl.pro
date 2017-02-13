#-------------------------------------------------
#
# Project created by QtCreator 2015-04-15T21:47:05
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += axcontainer
QT       += multimedia
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LicenseControl
TEMPLATE = app


SOURCES += main.cpp \
    licensecontrol.cpp \
    configdialog.cpp \
    selldialog.cpp \
    addlicensedialog.cpp \
    catalogdialog.cpp \
    canceldialog.cpp \
    newcustomerdialog.cpp \
    aboutdialog.cpp \
    changetextdialog.cpp \
    changelicensecountdialog.cpp \
    reportdialog.cpp \
    notificationdialog.cpp \
    softdialog.cpp \
    clientsdialog.cpp \
    trackingdialog.cpp \
    reportlicensesdialog.cpp

HEADERS  += \
    licensecontrol.h \
    configdialog.h \
    selldialog.h \
    addlicensedialog.h \
    catalogdialog.h \
    canceldialog.h \
    newcustomerdialog.h \
    aboutdialog.h \
    changetextdialog.h \
    changelicensecountdialog.h \
    reportdialog.h \
    notificationdialog.h \
    softdialog.h \
    clientsdialog.h \
    trackingdialog.h \
    reportlicensesdialog.h

FORMS    += \
    configdialog.ui \
    selldialog.ui \
    addlicensedialog.ui \
    catalogdialog.ui \
    canceldialog.ui \
    newcustomerdialog.ui \
    aboutdialog.ui \
    changetextdialog.ui \
    changelicensecountdialog.ui \
    reportdialog.ui \
    notificationdialog.ui \
    softdialog.ui \
    clientsdialog.ui \
    trackingdialog.ui \
    reportlicensesdialog.ui

RESOURCES += \
    Icons.qrc

include(3rdparty/qtxlsx/src/xlsx/qtxlsx.pri)

RC_FILE = app.rc
