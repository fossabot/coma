#include "fwuploaddialog.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../widgets/etableview.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QGridLayout>
#include <QMessageBox>
#include <QVBoxLayout>

FWUploadDialog::FWUploadDialog(QWidget *parent) : UDialog(parent)
{
    m_password = "admin";
    SetupUI();
}

void FWUploadDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    QPushButton *pb = WDFunc::NewPB(this, "", "Записать ПО в память " + tmps, this, &FWUploadDialog::LoadFW);
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, 1, 1, 1, 1);

    pb = WDFunc::NewPB(this, "", "Перейти на новое ПО", this, &FWUploadDialog::RunSoft);
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, 2, 1, 1, 1);

    lyout->addLayout(glyout, Qt::AlignTop);
    setLayout(lyout);
}

void FWUploadDialog::LoadFW()
{
    QByteArray ba;

    if (!checkPassword())
    {
        qCritical() << Error::GeneralError;
        return;
    }
    Error::Msg res = Files::LoadFromFile(Files::ChooseFileForOpen(this, "Program Version (*.hex)"), ba);
    if (res != Error::Msg::NoError)
    {
        qCritical() << "Ошибка файла ПО" << res;
        return;
    }

    auto firmwareS2 = S2::ParseHexToS2(ba);
    if (firmwareS2.isEmpty())
        qCritical() << Error::SizeError;
    setSuccessMsg("ПО записано успешно");
    BaseInterface::iface()->writeS2File(DataTypes::Firmware, &firmwareS2);
    return;
}

void FWUploadDialog::RunSoft()
{
    if (!checkPassword())
        return;
    setSuccessMsg("Переход на новое ПО выполнен успешно");
    BaseInterface::iface()->writeCommand(Queries::QC_StartFirmwareUpgrade);
}

// bool FWUploadDialog::WriteCheckPassword()
//{
//    KeyPressDialog dlg; // = new KeyPressDialog;
//    return dlg.CheckPassword("admin");
//}
