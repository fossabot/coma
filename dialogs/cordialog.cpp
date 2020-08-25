#include "../dialogs/cordialog.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/maindef.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../usb/commands.h"
#include "../widgets/emessagebox.h"
#include "../widgets/etableview.h"
#include "../widgets/wd_func.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStringListModel>
#include <QTabBar>
#include <QTabWidget>
#include <QVBoxLayout>

CorDialog::CorDialog(QWidget *parent) : AbstractCorDialog(parent)
{
    int i;

    CorBlock = new CorData;
    CorBlock->Phy_unb_init = 0;
    CorBlock->Iunb_init = 0;
    first = 0;

    /*MessageTimer = new QTimer;
    MessageTimer->setInterval(5000);
    connect(MessageTimer,SIGNAL(timeout()),this,SLOT(TimerTimeout()));*/

    for (i = 0; i < 3; i++)
    {
        CorBlock->C_init[i] = 0;
        CorBlock->Tg_init[i] = 0;
        CorBlock->corTg[i] = 0;
    }
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
    // MessageTimer->start();
}

CorDialog::~CorDialog()
{
}

void CorDialog::SetupUI()
{
    // QWidget *cp2 = new QWidget;
    QString tmps = "QDialog {background-color: " + QString(ACONFCLR) + ";}";
    setStyleSheet(tmps);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    ETableView *tv = new ETableView;
    tv->setObjectName("cor");
    int row = 0;
    QString paramcolor = MAINWINCLR;
    QPushButton *pb = new QPushButton();

    glyout->addWidget(WDFunc::NewLBL(this, "Начальные значения емкостей вводов, пФ:"), row, 1, 1, 1);

    for (int i = 0; i < 3; i++)
    {
        glyout->addWidget(WDFunc::NewSPB(this, QString::number(4000 + i), 0, 10000, 1, paramcolor), row, 2 + i, 1, 1);
    }

    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Начальные значения tg δ вводов, %:"), row, 1, 1, 1);

    for (int i = 0; i < 3; i++)
    {
        glyout->addWidget(WDFunc::NewSPB(this, QString::number(4003 + i), -10, 10, 2, paramcolor), row, 2 + i, 1, 1);
    }

    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Коррекция  tg δ вводов,%:"), row, 1, 1, 1);

    for (int i = 0; i < 3; i++)
    {
        glyout->addWidget(WDFunc::NewSPB(this, QString::number(4006 + i), -10, 10, 2, paramcolor), row, 2 + i, 1, 1);
    }
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Начальное действ. значение тока небаланса, мА:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, QString::number(4009), 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Начальное значение угла тока небаланса, град.:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, QString::number(4010), 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;

    // QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");

    pb = new QPushButton("Прочитать из модуля");
    connect(pb, SIGNAL(clicked()), this, SLOT(GetCorBdButton()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row, 1, 1, 2);

    pb = new QPushButton("Записать в модуль");
    connect(pb, SIGNAL(clicked()), this, SLOT(WriteCorBd()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row, 3, 1, 2);

    // row++;

    row++;

    pb = new QPushButton("Сбросить начальные значения");
    connect(pb, SIGNAL(clicked()), this, SLOT(ResetCor()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row, 1, 1, 2);

    pb = new QPushButton("Задать начальные значения");
    connect(pb, SIGNAL(clicked()), this, SLOT(WriteCor()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row, 3, 1, 2);

    // row++;

    row++;

    pb = new QPushButton("Прочитать значения из файла");
    connect(pb, SIGNAL(clicked()), this, SLOT(ReadFromFile()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row, 1, 1, 2);

    pb = new QPushButton("Сохранить значения в файл");
    connect(pb, SIGNAL(clicked()), this, SLOT(SaveToFile()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row, 3, 1, 2);

    // row++;

    // hlyout->addWidget(glyout,Qt::AlignTop);
    lyout->addLayout(glyout, Qt::AlignTop);
    lyout->addWidget(tv, 89);
    setLayout(lyout);
}

void CorDialog::FillBackCor()
{
    int i;
    QString tmps;

    WDFunc::SPBData(this, QString::number(4010), CorBlock->Phy_unb_init);
    WDFunc::SPBData(this, QString::number(4009), CorBlock->Iunb_init);

    for (i = 0; i < 3; i++)
    {
        WDFunc::SPBData(this, QString::number(4000 + i), CorBlock->C_init[i]);

        // WDFunc::LEData(this, "C_init1."+QString::number(i), tmps);
        // CorBlock->C_init[i]=ToFloat(tmps);
        WDFunc::SPBData(this, QString::number(4003 + i), CorBlock->Tg_init[i]);
        WDFunc::SPBData(this, QString::number(4006 + i), CorBlock->corTg[i]);
    }
}

void CorDialog::FillCor()
{
    int i;

    WDFunc::SetSPBData(this, QString::number(4010), CorBlock->Phy_unb_init);
    WDFunc::SetSPBData(this, QString::number(4009), CorBlock->Iunb_init);

    for (i = 0; i < 3; i++)
    {

        // WDFunc::SetLEData(this, "C_init1."+QString::number(i),
        // QString::number(CorBlock->C_init[i], 'f', 5));
        WDFunc::SetSPBData(this, QString::number(4000 + i), CorBlock->C_init[i]);
        WDFunc::SetSPBData(this, QString::number(4003 + i), CorBlock->Tg_init[i]);
        WDFunc::SetSPBData(this, QString::number(4006 + i), CorBlock->corTg[i]);
    }
}

void CorDialog::GetCorBd(int index)
{
    if (index == corDIndex)
    {
        if (MainInterface == I_USB)
        {
            if (Commands::GetBd(7, CorBlock, sizeof(CorData)) == NOERROR)
            {
                FillCor();
                EMessageBox::information(this, "INFO", "Прочитано успешно");
            }
        }
        /*        else if (MainInterface == I_RS485)
                {
                    ModBus::Information info;
                    info.size = (sizeof(CorData)/4);
                    info.adr = 4000;
                    emit RS485ReadCorBd(info);
                } */
        else if (MainInterface == I_ETHERNET)
        {
            emit CorReadRequest();
        }
    }
}
void CorDialog::GetCorBdButton()
{
    if (MainInterface == I_USB)
    {
        if (Commands::GetBd(7, CorBlock, sizeof(CorData)) == NOERROR)
        {
            FillCor();
            EMessageBox::information(this, "INFO", "Прочитано успешно");
        }
    }
    else if (MainInterface == I_RS485)
    {
        ModBus::Information info;
        info.size = (sizeof(CorData) / 4);
        info.adr = 4000;
        emit RS485ReadCorBd(info);
    }
    else if (MainInterface == I_ETHERNET)
    {
        emit CorReadRequest();
    }
}

void CorDialog::WriteCorBd()
{
    int i;
    quint32 adr[11] = { 910, 911, 912, 913, 914, 915, 916, 917, 918, 919, 920 };

    FillBackCor();

    if (WriteCheckPassword() == NOERROR)
    {
        if (MainInterface == I_ETHERNET)
        {
            for (i = 0; i < 11; i++)
            {
                float corblocki;
                memcpy(&corblocki, reinterpret_cast<float *>(CorBlock) + i, sizeof(float));
                emit SendCom50(adr[i], corblocki);
                TimeFunc::Wait(300);
            }
        }
        else if (MainInterface == I_RS485)
        {
            ModBus::Information info;
            info.size = (sizeof(CorData) / 4);
            info.adr = adr[0];
            emit RS485WriteCorBd(info, (float *)CorBlock);
        }
        else if (MainInterface == I_USB)
        {
            if (Commands::WriteBd(7, CorBlock, sizeof(CorData)) == NOERROR)
                EMessageBox::information(this, "INFO", "Записано успешно");
            else
                EMessageBox::information(this, "INFO", "Ошибка");

            if (Commands::GetBd(7, CorBlock, sizeof(CorBlock)) == NOERROR)
                FillCor();
        }
    }
}

void CorDialog::WriteCor()
{
    if (WriteCheckPassword() == NOERROR)
    {
        if (MainInterface == I_ETHERNET)
        {
            emit SendCom45(SETINITREG);
            EMessageBox::information(this, "INFO", "Задано успешно");
            emit CorReadRequest();
        }
        else if (MainInterface == I_RS485)
        {
            ModBus::Information info;
            info.size = 1;
            info.adr = SETINITREG;
            emit RS485WriteCorBd(info, nullptr);
            EMessageBox::information(this, "INFO", "Задано успешно");
            info.size = (sizeof(CorData) / 4);
            info.adr = 4000;
            emit RS485ReadCorBd(info);
        }
        else if (MainInterface == I_USB)
        {
            if (Commands::WriteCom(1) == NOERROR) // задание общей коррекции
            {
                if (Commands::GetBd(7, CorBlock, sizeof(CorData)) == NOERROR)
                {
                    FillCor();
                    EMessageBox::information(this, "INFO", "Задано и прочитано успешно");
                }
            }
            else
                EMessageBox::information(this, "INFO", "Ошибка");
        }
    }
}

void CorDialog::SetCor()
{
    if (MainInterface == I_ETHERNET)
    {
        emit SendCom45(903);
    }
    else if (MainInterface == I_USB)
    {
        if (Commands::WriteCom(4) == NOERROR)
            EMessageBox::information(this, "INFO", "Записано успешно");
        else
            EMessageBox::information(this, "INFO", "Ошибка");
    }
}

void CorDialog::ResetCor()
{
    if (WriteCheckPassword() == NOERROR)
    {
        if (MainInterface == I_ETHERNET)
        {
            emit SendCom45(CLEARREG);
        }
        else if (MainInterface == I_RS485)
        {
            ModBus::Information info;
            info.size = 1;
            info.adr = CLEARREG;
            emit RS485WriteCorBd(info, nullptr);
        }
        else if (MainInterface == I_USB)
        {
            if (Commands::WriteCom(5) == NOERROR)
                EMessageBox::information(this, "INFO", "Сброшено успешно");
            else
                EMessageBox::information(this, "INFO", "Ошибка");

            if (Commands::GetBd(7, CorBlock, sizeof(CorBlock)) == NOERROR)
                FillCor();
        }
    }
}

float CorDialog::ToFloat(QString text)
{
    bool ok;
    float tmpf;
    tmpf = text.toFloat(&ok);
    if (!ok)
    {
        ERMSG("Значение " + text + " не может быть переведено во float");
        return 0;
    }
    return tmpf;
}

void CorDialog::MessageOk()
{
    EMessageBox::information(this, "INFO", "Записано успешно");
}

void CorDialog::UpdateFlCorData(IEC104Thread::FlSignals104 *Signal)
{
    IEC104Thread::FlSignals104 sig = *new IEC104Thread::FlSignals104;
    int i;

    if (((Signal)->fl.SigAdr >= 4000) && ((Signal)->fl.SigAdr <= 4010))
    {
        for (i = 0; i < Signal->SigNumber; i++)
        {
            sig = *(Signal + i);
            // if((Signal+i)->fl.SigAdr >= 1000 || (Signal+i)->fl.SigAdr <= 1009)
            FillBd(
                this, QString::number((Signal + i)->fl.SigAdr), WDFunc::StringValueWithCheck((Signal + i)->fl.SigVal));
        }

        if (first)
            EMessageBox::information(this, "INFO", "Прочитано успешно");
        else
            first = 1;
    }
}

void CorDialog::FillBd(QWidget *parent, QString Name, QString Value)
{
    float fl;
    QLocale german(QLocale::German);
    fl = german.toDouble(Value);
    WDFunc::SetSPBData(parent, Name, fl);
}

void CorDialog::ModBusUpdateCorData(QList<ModBus::SignalStruct> Signal)
{
    int i = 0;

    if (Signal.size() > 0)
    {
        if (Signal.at(0).SigAdr == 4000)
        {
            for (i = 0; i < Signal.size(); ++i)
            {
                FillBd(this, QString::number(Signal.at(i).SigAdr), WDFunc::StringValueWithCheck(Signal.at(i).flVal));
            }
            EMessageBox::information(this, "INFO", "Прочитано успешно");
        }
    }
}

/*void CorDialog::ModbusCorDataWritten()
{
    EMessageBox::information(this, "INFO", "Записано успешно");
} */

void CorDialog::SaveToFile()
{
    int res = NOERROR;
    QByteArray ba;
    FillBackCor();
    ba.resize(sizeof(*CorBlock));
    memcpy(&(ba.data()[0]), CorBlock, sizeof(*CorBlock));
    res = Files::SaveToFile(Files::ChooseFileForSave(this, "Tune files (*.cor)", "cor"), ba, sizeof(*CorBlock));
    switch (res)
    {
    case Files::ER_NOERROR:
        EMessageBox::information(this, "Внимание", "Файл коэффициентов коррекции записан успешно!");
        break;
    case Files::ER_FILEWRITE:
        EMessageBox::error(this, "Ошибка", "Ошибка при записи файла!");
        break;
    case Files::ER_FILENAMEEMP:
        EMessageBox::error(this, "Ошибка", "Пустое имя файла!");
        break;
    case Files::ER_FILEOPEN:
        EMessageBox::error(this, "Ошибка", "Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void CorDialog::ReadFromFile()
{
    QByteArray ba;
    ba.resize(sizeof(*CorBlock));

    int res = Files::LoadFromFile(Files::ChooseFileForOpen(this, "Tune files (*.cor)"), ba);
    if (res != Files::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка при загрузке файла");
        ERMSG("Ошибка при загрузке файла");
        return;
    }

    memcpy(CorBlock, &(ba.data()[0]), sizeof(*CorBlock));

    FillCor();
    EMessageBox::information(this, "Внимание", "Загрузка прошла успешно!");
}

int CorDialog::WriteCheckPassword()
{
    ok = false;
    StdFunc::ClearCancel();
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
    connect(dlg, SIGNAL(Finished(QString)), this, SLOT(WritePasswordCheck(QString)));
    connect(this, SIGNAL(WritePasswordChecked()), &PasswordLoop, SLOT(quit()));
    dlg->deleteLater();
    dlg->show();
    PasswordLoop.exec();
    if (StdFunc::IsCancelled())
        return GENERALERROR;
    if (!ok)
    {
        EMessageBox::error(this, "Неправильно", "Пароль введён неверно");
        return GENERALERROR;
    }
    return NOERROR;
}

void CorDialog::WritePasswordCheck(QString psw)
{
    if (psw == "121941")
        ok = true;
    else
        ok = false;
    emit WritePasswordChecked();
}

void CorDialog::TimerTimeout()
{
    MessageTimer->stop();
    // first = 1;
}

/*void CorDialog::WriteCorMessageOk()
{
   EMessageBox::information(this, "Успешно", "Записано успешно!");
} */

void CorDialog::ErrorRead()
{
    EMessageBox::information(this, "Ошибка", "Ошибка чтения");
}
