#-------------------------------------------------
#
# Project created by QtCreator 2019-06-13T12:18:20
#
#-------------------------------------------------

QMAKE_TARGET_COMPANY = EvelSoft
QMAKE_TARGET_COPYRIGHT = EvelSoft
QMAKE_TARGET_PRODUCT = AVTUK-S
RC_ICONS = ../coma.ico
CONFIG += c++11
VERSION = 0.2.0

QT       += core gui printsupport network serialport qml widgets testlib svg

TARGET = AVM-Service
DEFINES += PROGNAME='\\"AVM-Service\\"'
DEFINES += PROGCAPTION='\\"AVM-Service\\040v\\040"$$VERSION"\\040\\"'
DEFINES += COMAVERSION='\\"$$VERSION\\"'
DEFINES += DEVICETYPE=1 # 1 - module, 2 - pribor, for diagnostic messages
DEFINES += SOFTDEVELOPER='\\"EvelSoft\\"'
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# DEFINES += DEBUG
TEMPLATE = app


SOURCES += \
    ../alarm/abstractalarm.cpp \
    ../alarm/alarmclass.cpp \
    ../alarm/alarmstateall.cpp \
    ../alarm/alarmwidget.cpp \
    ../alarm/avaralarmkiv.cpp \
    ../alarm/avaralarmktf.cpp \
    ../alarm/warnalarmkiv.cpp \
    ../alarm/warnalarmktf.cpp \
    ../check/checkdialogharmonicktf.cpp \
    ../check/checkdialogkdv.cpp \
    ../check/checkdialogkiv.cpp \
    ../check/checkdialogktf.cpp \
    ../check/checkharmonicktf.cpp \
    ../check/checkkdv.cpp \
    ../check/checkkiv.cpp \
    ../check/checkktf.cpp \
    ../config/confdialogkdv.cpp \
    ../config/confdialogkiv.cpp \
    ../config/confdialogktf.cpp \
    ../config/confdialogkxx.cpp \
    ../config/configkdv.cpp \
    ../config/configkiv.cpp \
    ../config/configktf.cpp \
    ../config/configkxx.cpp \
    ../dialogs/abstractcordialog.cpp \
    ../dialogs/connectdialog.cpp \
    ../dialogs/cordialogktf.cpp \
    ../dialogs/infodialog.cpp \
    ../dialogs/mnktime.cpp \
    ../gen/journals.cpp \
    ../gen/logclass.cpp \
    ../gen/maindef.cpp \
    ../modbus/serialport.cpp \
    ../usb/usb.cpp \
    coma.cpp \
    main.cpp \
    ../check/check.cpp \
    ../check/eabstractcheckdialog.cpp \
    ../config/abstractconfdialog.cpp \
    ../config/confdialog.cpp \
    ../config/config.cpp \
    ../dialogs/cordialog.cpp \
    ../dialogs/errordialog.cpp \
    ../dialogs/fwupdialog.cpp \
    ../dialogs/journalsdialog.cpp \
    ../dialogs/keypressdialog.cpp \
    ../dialogs/settingsdialog.cpp \
    ../gen/error.cpp \
    ../gen/files.cpp \
    ../gen/modulebsi.cpp \
    ../gen/report.cpp \
    ../gen/s2.cpp \
    ../gen/stdfunc.cpp \
    ../gen/timefunc.cpp \
    ../iec104/ethernet.cpp \
    ../iec104/iec104.cpp \
    ../models/etableitem.cpp \
    ../models/etablemodel.cpp \
    ../models/getoscpbdelegate.cpp \
    ../modbus/modbus.cpp \
    ../usb/commands.cpp \
    ../usb/eabstractprotocomchannel.cpp \
    ../usb/eusbhid.cpp \
    ../widgets/ecombobox.cpp \
    ../widgets/emessagebox.cpp \
    ../widgets/errorprotocolwidget.cpp \
    ../widgets/estackedwidget.cpp \
    ../widgets/etableview.cpp \
    ../widgets/etabwidget.cpp \
    ../widgets/lineeditfield.cpp \
    ../widgets/passwordlineedit.cpp \
    ../widgets/signalchoosewidget.cpp \
    ../widgets/waitwidget.cpp \
    ../widgets/wd_func.cpp

HEADERS += \
    ../alarm/abstractalarm.h \
    ../alarm/alarmclass.h \
    ../alarm/alarmstateall.h \
    ../alarm/alarmwidget.h \
    ../alarm/avaralarmkiv.h \
    ../alarm/avaralarmktf.h \
    ../alarm/warnalarmkiv.h \
    ../alarm/warnalarmktf.h \
    ../check/checkdialogharmonicktf.h \
    ../check/checkdialogkdv.h \
    ../check/checkdialogkiv.h \
    ../check/checkdialogktf.h \
    ../check/checkharmonicktf.h \
    ../check/checkkdv.h \
    ../check/checkkiv.h \
    ../check/checkktf.h \
    ../config/confdialogkdv.h \
    ../config/confdialogkiv.h \
    ../config/confdialogktf.h \
    ../config/confdialogkxx.h \
    ../config/configkdv.h \
    ../config/configkiv.h \
    ../config/configktf.h \
    ../config/configkxx.h \
    ../dialogs/abstractcordialog.h \
    ../dialogs/connectdialog.h \
    ../dialogs/cordialogktf.h \
    ../dialogs/infodialog.h \
    ../dialogs/mnktime.h \
    ../gen/journals.h \
    ../gen/logclass.h \
    ../modbus/serialport.h \
    ../usb/usb.h \
    coma.h \
    ../check/check.h \
    ../check/eabstractcheckdialog.h \
    ../config/abstractconfdialog.h \
    ../config/confdialog.h \
    ../config/config.h \
    ../dialogs/cordialog.h \
    ../dialogs/errordialog.h \
    ../dialogs/fwupdialog.h \
    ../dialogs/journalsdialog.h \
    ../dialogs/keypressdialog.h \
    ../dialogs/settingsdialog.h \
    ../gen/colors.h \
    ../gen/error.h \
    ../gen/files.h \
    ../gen/maindef.h \
    ../gen/modulebsi.h \
    ../gen/report.h \
    ../gen/s2.h \
    ../gen/stdfunc.h \
    ../gen/timefunc.h \
    ../iec104/ethernet.h \
    ../iec104/iec104.h \
    ../models/etableitem.h \
    ../models/etablemodel.h \
    ../models/getoscpbdelegate.h \
    ../usb/commands.h \
    ../usb/eabstractprotocomchannel.h \
    ../usb/eusbhid.h \
    ../widgets/ecombobox.h \
    ../widgets/emessagebox.h \
    ../widgets/errorprotocolwidget.h \
    ../widgets/estackedwidget.h \
    ../widgets/etableview.h \
    ../widgets/etabwidget.h \
    ../widgets/keypress.h \
    ../widgets/lineeditfield.h \
    ../widgets/passwordlineedit.h \
    ../widgets/signalchoosewidget.h \
    ../widgets/waitwidget.h \
    ../widgets/wd_func.h \
    ../modbus/modbus.h


IMAGE_FILE += \
    $$PWD/../images

ERROR_FILES += \
    $$PWD/../errors



INCLUDEPATH += $$PWD/../../includes

equals(QMAKE_PLATFORM, win32)
{
    contains(QMAKE_TARGET.arch, x86_64) {
        message("x64 build")
       ## Windows x64 (64bit) specific build here
       CONFIG(debug, debug|release) {
       LIBS += -L$$PWD/../../libs/win64/debug/ -llimereportd -lliblzma -lhidapi -lqt5xlsxd
       DESTDIR = $${PWD}/../../build/win64/debug
       } else {
       LIBS += -L$$PWD/../../libs/win64/release/ -llimereport -lliblzma -lhidapi -lqt5xlsx
       DESTDIR = $${PWD}/../../build/win64/release
       LIBS_FILES += \
       $$PWD/../../libs/win64/release/hidapi.dll \
       $$PWD/../../libs/win64/release/liblzma.dll \
       $$PWD/../../libs/win64/release/limereport.dll \
       $$PWD/../../libs/win64/release/Qt5Xlsx.dll \
       $$PWD/../../libs/win64/release/QtZint.dll
       }
    } else {
        message("x86 build")
        ## Windows x86 (32bit) specific build here
        CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../libs/win32/debug/ -llimereportd -lliblzma -lhidapi -lqt5xlsxd
        DESTDIR = $${PWD}/../../build/win32/debug
        } else {
        LIBS += -L$$PWD/../../libs/win32/release/ -llimereport -lliblzma -lhidapi -lqt5xlsx
        DESTDIR = $${PWD}/../../build/win32/release
        LIBS_FILES += \
        $$PWD/../../libs/win32/release/hidapi.dll \
        $$PWD/../../libs/win32/release/liblzma.dll \
        $$PWD/../../libs/win32/release/limereport.dll \
        $$PWD/../../libs/win32/release/Qt5Xlsx.dll \
        $$PWD/../../libs/win32/release/QtZint.dll
        }
    }
}

unix: LIBS += -L$$PWD/libs/win32/debug/ -llimereportd -lliblzma -lqt5xlsxd


# copies the given files to the destination directory
defineTest(copyToDestDir) {
    files = $$1
    dir = $$2
    # replace slashes in destination path for Windows
    win32:dir ~= s,/,\\,g

    for(file, files) {
        # replace slashes in source path for Windows
        win32:file ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_quote($$file) $$shell_quote($$dir) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

copyToDestDir($$IMAGE_FILE, $$DESTDIR/images/)
copyToDestDir($$ERROR_FILES, $$DESTDIR/errors/)
copyToDestDir($$LIBS_FILES, $$DESTDIR/)