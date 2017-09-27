#-------------------------------------------------
#
# Project created by QtCreator 2015-03-02T10:43:56
#
#-------------------------------------------------

QMAKE_TARGET_COMPANY = EvelSoft
QMAKE_TARGET_COPYRIGHT = EvelSoft
QMAKE_TARGET_PRODUCT = AVTUK-S
RC_ICONS = ../coma.ico
CONFIG += c++11
VERSION = 2.0.79

QT       += core gui network serialport printsupport

TARGET = avtuk-s_f
#DEFINES += MODULE_TYPEB=0xA1
DEFINES += PROGNAME='\\"AVTUK-S\\"'
DEFINES += PROGCAPTION='\\"AVTUK-Service-F\\040v2.0.79\\"'
DEFINES += DEVICETYPE=1 # 1 - module, 2 - pribor, for diagnostic messages
DEFINES += PROGSIZE=4 # 1 - SMALL (only for users), 2 - MEDIUM (for mid-class users), 3 - LARGE (for developers of modules), 4 - FULL (for developer of the prog)
#DEFINES += COMPORTENABLE # enable virtual com port driver
DEFINES += USBENABLE # enable usb hid driver
TEMPLATE = app

SOURCES += \
    main.cpp\
    coma.cpp \
    ../commands.cpp \
    ../eabstractprotocomchannel.cpp \
    ../eusbcom.cpp \
    ../eusbhid.cpp \
    ../log.cpp \
    ../mainwindow.cpp \
    ../publicclass.cpp \
    ../check/check80.cpp \
    ../check/checka1.cpp \
    ../check/checkdialog21.cpp \
    ../check/checkdialog80.cpp \
    ../check/checkdialoga1.cpp \
    ../check/eabstractcheckdialog.cpp \
    ../config/abstractconfdialog.cpp \
    ../config/abstractconfdialog2x.cpp \
    ../config/abstractconfdialog3x.cpp \
    ../config/confdialog.cpp \
    ../config/confdialog21.cpp \
    ../config/confdialog31.cpp \
    ../config/confdialog35.cpp \
    ../config/confdialog80.cpp \
    ../config/confdialoga1.cpp \
    ../config/config.cpp \
    ../config/config21.cpp \
    ../config/config2x.cpp \
    ../config/config31.cpp \
    ../config/config35.cpp \
    ../config/config3x.cpp \
    ../config/config80.cpp \
    ../config/configa1.cpp \
    ../dialogs/a1dialog.cpp \
    ../dialogs/downloaddialog.cpp \
    ../dialogs/errordialog.cpp \
    ../dialogs/fwupdialog.cpp \
    ../dialogs/hiddendialog.cpp \
    ../dialogs/infodialog.cpp \
    ../dialogs/keypressdialog.cpp \
    ../dialogs/oscdialog.cpp \
    ../dialogs/settingsdialog.cpp \
    ../iec104/iec104.cpp \
    ../iec104/ethernet.cpp \
    ../tune/eabstracttunedialog.cpp \
    ../tune/tunedialog21.cpp \
    ../tune/tunedialog80.cpp \
    ../tune/tunedialoga1.cpp \
    ../tune/tunedialoga1dn.cpp \
    ../widgets/errorprotocolwidget.cpp \
    ../widgets/getoscpbdelegate.cpp \
    ../widgets/messagebox.cpp \
    ../widgets/mytabwidget.cpp \
    ../widgets/mystackedwidget.cpp \
    ../widgets/etableitem.cpp \
    ../widgets/etablemodel.cpp \
    ../widgets/s_tqcombobox.cpp \
    ../widgets/s_tqtableview.cpp \
    ../widgets/waitwidget.cpp \
    ../widgets/wd_func.cpp \
    ../check/check21.cpp \
    ../config/confdialog85.cpp \
    ../config/config85.cpp

HEADERS  += \
    coma.h \
    ../commands.h \
    ../eabstractprotocomchannel.h \
    ../eusbcom.h \
    ../eusbhid.h \
    ../log.h \
    ../mainwindow.h \
    ../publicclass.h \
    ../check/check80.h \
    ../check/checka1.h \
    ../check/checkdialog21.h \
    ../check/checkdialog80.h \
    ../check/checkdialoga1.h \
    ../check/eabstractcheckdialog.h \
    ../config/abstractconfdialog.h \
    ../config/abstractconfdialog2x.h \
    ../config/abstractconfdialog3x.h \
    ../config/confdialog.h \
    ../config/confdialog21.h \
    ../config/confdialog31.h \
    ../config/confdialog35.h \
    ../config/confdialog80.h \
    ../config/confdialoga1.h \
    ../config/config.h \
    ../config/config21.h \
    ../config/config2x.h \
    ../config/config31.h \
    ../config/config35.h \
    ../config/config3x.h \
    ../config/config80.h \
    ../config/configa1.h \
    ../dialogs/a1dialog.h \
    ../dialogs/downloaddialog.h \
    ../dialogs/errordialog.h \
    ../dialogs/fwupdialog.h \
    ../dialogs/hiddendialog.h \
    ../dialogs/infodialog.h \
    ../dialogs/keypressdialog.h \
    ../dialogs/oscdialog.h \
    ../dialogs/settingsdialog.h \
    ../iec104/ethernet.h \
    ../iec104/iec104.h \
    ../tune/eabstracttunedialog.h \
    ../tune/tunedialog21.h \
    ../tune/tunedialog80.h \
    ../tune/tunedialoga1.h \
    ../tune/tunedialoga1dn.h \
    ../widgets/getoscpbdelegate.h \
    ../widgets/mytabwidget.h \
    ../widgets/errorprotocolwidget.h \
    ../widgets/messagebox.h \
    ../widgets/mystackedwidget.h \
    ../widgets/etableitem.h \
    ../widgets/etablemodel.h \
    ../widgets/s_tqcombobox.h \
    ../widgets/s_tqtableview.h \
    ../widgets/waitwidget.h \
    ../widgets/wd_func.h \
    ../check/check21.h \
    ../config/confdialog85.h \
    ../config/config85.h

RESOURCES += \
    res.qrc

INCLUDEPATH += $$PWD/../../includes

win32 {

    ## Windows common build here

    !contains(QMAKE_TARGET.arch, x86_64) {
        ## Windows x86 (32bit) specific build here
        CONFIG(release, debug|release): LIBS += -L$$PWD/../../libs/win32/release/ -llimereport -lliblzma -lhidapi -lqt5xlsx
        CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libs/win32/debug/ -llimereport -lliblzma -lhidapi -lqt5xlsxd
    } else {
        ## Windows x64 (64bit) specific build here
        CONFIG(release, debug|release): LIBS += -L$$PWD/../../libs/win64/release/ -llimereport -lliblzma -lhidapi -lqt5xlsx
        CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libs/win64/debug/ -llimereport -lliblzma -lhidapi -lqt5xlsxd
    }
}
unix: LIBS += -L$$PWD/libs/win32/debug/ -llimereport -lliblzma -lhidapi -lqt5xlsxd
