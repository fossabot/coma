#include "module.h"

#include "../check/checkkdvdialog.h"
#include "../check/checkkdvharmonicdialog.h"
#include "../check/checkkdvvibrdialog.h"
#include "../check/checkkivdialog.h"
#include "../check/checkktfdialog.h"
#include "../check/checkktfharmonicdialog.h"
#include "../config/confkdvdialog.h"
#include "../config/confkivdialog.h"
#include "../config/confktfdialog.h"
#include "../dialogs/fwuploaddialog.h"
#include "../dialogs/infodialog.h"
#include "../dialogs/journalsdialog.h"
#include "../dialogs/timedialog.h"
#include "../gen/udialog.h"
#include "../startup/startupkdvdialog.h"
#include "../startup/startupkivdialog.h"
#include "../startup/startupktfdialog.h"
#ifdef AVM_DEBUG
//#include "../tune/kiv/tunekdvdialog.h"
#include "../tune/kiv/tunekivdialog.h"
//#include "../tune/kiv/tunektfdialog.h"
#endif
#include "alarmkdv.h"
#include "alarmkiv.h"
#include "alarmktf.h"
#include "journkdv.h"
#include "journkiv.h"
#include "journktf.h"
#include "warnkdv.h"
#include "warnkiv.h"
#include "warnktf.h"

Module::Module(QObject *parent) : QObject(parent)
{
    m_Dialogs.clear();
    m_oldTabIndex = m_currentTabIndex = 0;
}

Module *Module::createModule(QTimer *updateTimer)
{
    Journals *JOUR;
    Module *m = new Module;
    S2ConfigType *s2Config = new S2ConfigType;
    quint16 typeb = Board::GetInstance().typeB();
    if (Board::GetInstance().getBaseBoardsList().contains(typeb)) // there must be two-part module
    {
        quint16 typem = Board::GetInstance().typeM();
        Q_UNUSED(typem)
        switch (typeb)
        {
        case Board::BaseBoards::MTB_00:
            /*
                str = (checkMDialog == nullptr) ? "Текущие параметры" : "Текущие параметры\nБазовая";
                if (checkBDialog != nullptr)
                {
                    checkBDialog->setMinimumHeight(500);
                    MainTW->addTab(checkBDialog, str);
                    CheckIndex = MainTW->indexOf(checkBDialog);
                }
                str = (checkBDialog == nullptr) ? "Текущие параметры" : "Текущие параметры\nМезонин";
                if (checkMDialog != nullptr)
                    MainTW->addTab(checkMDialog, str);
            */
        default:
            break;
        }
    }
    else
    {
        quint16 mtype = Board::GetInstance().type();
        switch (mtype)
        {
        case Board::DeviceModel::KIV:
        {
            JOUR = new JournKIV;
            ConfigKIV *CKIV = new ConfigKIV(s2Config);
            m->addDialogToList(new ConfKIVDialog(CKIV), "Конфигурирование", "conf1");
            CheckKIVDialog *cdkiv = new CheckKIVDialog;
            m->addDialogToList(cdkiv, "Проверка");
#ifdef AVM_DEBUG
            TuneKIV *TKIV = new TuneKIV(0, s2Config);
            m->addDialogToList(new TuneKIVDialog(CKIV, TKIV), "Регулировка");
#endif
            m->addDialogToList(new StartupKIVDialog, "Начальные значения");
            m->m_Warn = new WarnKIV;
            m->m_Alarm = new AlarmKIV;
            connect(m->m_Warn, &Warn::updateWarn, cdkiv, &AbstractCheckDialog::SetWarnColor);
            connect(m->m_Alarm, &Alarm::updateAlarm, cdkiv, &AbstractCheckDialog::SetAlarmColor);
        }
        case Board::DeviceModel::KTF:
        {
            JOUR = new JournKTF;
            ConfigKTF *CKTF = new ConfigKTF(s2Config);
            m->addDialogToList(new ConfKTFDialog(CKTF), "Конфигурирование", "conf1");
            CheckKTFDialog *cdktf = new CheckKTFDialog;
            m->addDialogToList(new CheckKTFDialog);
#ifdef AVM_DEBUG
            //            TuneKTF *TKTF = new TuneKTF(0, s2Config);
//                        m->addDialogToList(new TuneKTFDialog(CKTF, TKTF));
#endif
            m->addDialogToList(new StartupKTFDialog, "Старение изоляции");
            m->addDialogToList(new CheckKTFHarmonicDialog, "Гармоники");
            m->m_Warn = new WarnKTF;
            m->m_Alarm = new AlarmKTF;
            connect(m->m_Warn, &Warn::updateWarn, cdktf, &AbstractCheckDialog::SetWarnColor);
            connect(m->m_Alarm, &Alarm::updateAlarm, cdktf, &AbstractCheckDialog::SetAlarmColor);
            break;
        }
        case Board::DeviceModel::KDV:
        {
            JOUR = new JournKDV;
            ConfigKDV *CKDV = new ConfigKDV(s2Config);
            m->addDialogToList(new ConfKDVDialog(CKDV), "Конфигурирование", "conf1");
            CheckKDVDialog *cdkdv = new CheckKDVDialog;
            m->addDialogToList(new CheckKDVDialog);
#ifdef AVM_DEBUG
            TuneKDV *TKDV = new TuneKDV(0, s2Config);
            m->addDialogToList(new TuneKDVDialog(CKDV, TKDV));
#endif
            m->addDialogToList(new StartupKDVDialog, "Старение изоляции");
            m->addDialogToList(new CheckKDVHarmonicDialog, "Гармоники");
            m->addDialogToList(new CheckKDVVibrDialog, "Вибрации");
            //            VibrDialog = new CheckDialogVibrKDV(BoardTypes::BT_BASE);
            //            connect(BdaTimer, &QTimer::timeout, VibrDialog, &AbstractCheckDialog::USBUpdate);
            m->m_Warn = new WarnKDV;
            m->m_Alarm = new AlarmKDV;
            connect(m->m_Warn, &Warn::updateWarn, cdkdv, &AbstractCheckDialog::SetWarnColor);
            connect(m->m_Alarm, &Alarm::updateAlarm, cdkdv, &AbstractCheckDialog::SetAlarmColor);
            break;
        }
        default:
            assert(false);
        }
    }

    TimeDialog *tdlg = new TimeDialog;
    m->addDialogToList(tdlg, "Время", "time");

    if (Board::GetInstance().interfaceType() != Board::InterfaceType::USB)
    {
        //        m->addDialogToList(new JournalDialog(JOUR, ), "Журналы");
    }
    if (Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
        m->addDialogToList(new FWUploadDialog, "Загрузка ВПО");

    m->addDialogToList(new InfoDialog, "О приборе", "info");

    QList<UDialog *> dlgs = m->dialogs();
    foreach (UDialog *d, dlgs)
    {
        connect(updateTimer, &QTimer::timeout, d, &UDialog::update);
        d->setUpdatesDisabled();
    }
    return m;
}

QList<UDialog *> Module::dialogs()
{
    QList<UDialog *> list = m_Dialogs;
    return list;
}

QList<UDialog *> Module::confDialogs()
{
    QList<UDialog *> list;
    foreach (UDialog *dlg, m_Dialogs)
    {
        if (dlg->objectName().contains("conf"))
            list.append(dlg);
    }
    return list;
}

void Module::addDialogToList(UDialog *dlg, const QString &caption, const QString &name)
{
    dlg->setObjectName(name);
    dlg->setCaption(caption);
    m_Dialogs.append(dlg);
}

Alarm *Module::getAlarm() { return m_Alarm; }

Warn *Module::getWarn() { return m_Warn; }

void Module::parentTWTabClicked(int index)
{
    if (index == m_currentTabIndex) // to prevent double function invocation by doubleclicking on tab
        return;
    m_currentTabIndex = index;

    QDialog *dlg = m_Dialogs.at(m_oldTabIndex);
    UDialog *udlg = qobject_cast<UDialog *>(dlg);
    if (udlg)
        udlg->setUpdatesDisabled();
    dlg = m_Dialogs.at(m_currentTabIndex);
    udlg = qobject_cast<UDialog *>(dlg);
    if (udlg)
        udlg->setUpdatesEnabled();
    udlg->update();
    m_oldTabIndex = m_currentTabIndex;
}

void Module::setDefConf()
{
    foreach (QDialog *dlg, m_Dialogs)
    {
        if (dlg->objectName().contains("conf"))
        {
            static_cast<AbstractConfDialog *>(dlg)->SetDefConf();
            static_cast<AbstractConfDialog *>(dlg)->Fill();
        }
    }
}

void Module::closeDialogs()
{
    for (auto &i : m_Dialogs)
        i->close();
}
