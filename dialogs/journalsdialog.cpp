#include <QDateTime>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QApplication>
#include <QSortFilterProxyModel>
#include <QFileDialog>

#include "../widgets/emessagebox.h"
#include "../gen/maindef.h"
#include "../models/getoscpbdelegate.h"
#include "../widgets/wd_func.h"
#include "../gen/stdfunc.h"
#include "../gen/error.h"
#include "../gen/timefunc.h"
#include "../dialogs/journalsdialog.h"
#include "../gen/colors.h"
#include "../gen/files.h"
#include "../gen/commands.h"
#include "../config/confdialog.h"

JournalDialog::JournalDialog(IEC104 *iec, QWidget *parent) : QDialog(parent)
{
    JourFuncs = new Journals;
    ProxyWorkModel = new QSortFilterProxyModel;
    ProxySysModel = new QSortFilterProxyModel;
    ProxyMeasModel = new QSortFilterProxyModel;
    JourFuncs->SetProxyModels(ProxyWorkModel, ProxySysModel, ProxyMeasModel);
//    JourFuncs->SetParentWidget(this);
    connect(JourFuncs,SIGNAL(Done(QString)),this,SLOT(Done(QString)));
    connect(JourFuncs,SIGNAL(Error(QString)),this,SLOT(Error(QString)));
    connect(JourFuncs,SIGNAL(ModelReady(ETableModel *)),this,SLOT(SetModel(ETableModel *)));
//    connect(JourFuncs,SIGNAL(ProxyModelReady(QSortFilterProxyModel *)),this,SLOT(SetProxyModel(QSortFilterProxyModel *)));
    connect(JourFuncs,SIGNAL(ReadJour(char)), iec, SLOT(SelectFile(char)));
    connect(iec,SIGNAL(SendJourSysfromiec104(QByteArray)), JourFuncs, SLOT(FillSysJour(QByteArray)));
    connect(iec,SIGNAL(SendJourWorkfromiec104(QByteArray)), JourFuncs, SLOT(FillWorkJour(QByteArray)));
    connect(iec,SIGNAL(SendJourMeasfromiec104(QByteArray)), JourFuncs, SLOT(FillMeasJour(QByteArray)));
    connect(this,SIGNAL(StartGetJour()),JourFuncs,SLOT(StartGetJour()));
    connect(this,SIGNAL(StartSaveJour(int, QAbstractItemModel *, QString)),JourFuncs,SLOT(StartSaveJour(int, QAbstractItemModel *, QString)));
    connect(this,SIGNAL(StartReadFile()),JourFuncs,SLOT(ReadJourFileAndProcessIt()));
    JourFuncs->moveToThread(&JourThread);
    JourThread.start();
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

JournalDialog::~JournalDialog()
{
    JourThread.quit();
    JourThread.wait();
}

void JournalDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
/*    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout; */
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";

/*    QTabWidget *work = new QTabWidget;
    work->tabBar()->setStyleSheet(ConfTWss);
    hlyout->addWidget(WDFunc::NewPB(this, "gj." + QString::number(Journals::JOURWORK), "Получить рабочий журнал", this, SLOT(TryGetJourByUSB())));
    hlyout->addWidget(WDFunc::NewPB(this, "ej." + QString::number(Journals::JOURWORK), "Стереть рабочий журнал", this, SLOT(EraseJour())));
    hlyout->addWidget(WDFunc::NewPB(this, "sj." + QString::number(Journals::JOURWORK), "Сохранить журнал в файл", this, SLOT(SaveJour())));
    vlyout->addLayout(hlyout);
    vlyout->addWidget(WDFunc::NewTV(this, "work", nullptr), 89);
    work->setLayout(vlyout);
    work->setStyleSheet(ConfTWss);

    QTabWidget *system = new QTabWidget;
    system->tabBar()->setStyleSheet(ConfTWss);

    hlyout = new QHBoxLayout;
    vlyout = new QVBoxLayout;
    hlyout->addWidget(WDFunc::NewPB(this, "gj." + QString::number(Journals::JOURSYS), "Получить системный журнал", this, SLOT(TryGetJourByUSB())));
    hlyout->addWidget(WDFunc::NewPB(this, "ej." + QString::number(Journals::JOURSYS), "Стереть системный журнал", this, SLOT(EraseJour())));
    hlyout->addWidget(WDFunc::NewPB(this, "sj." + QString::number(Journals::JOURSYS), "Сохранить журнал в файл", this, SLOT(SaveJour())));
    vlyout->addLayout(hlyout);
    vlyout->addWidget(WDFunc::NewTV(this, "system", nullptr), 89);
    system->setLayout(vlyout);
    system->setStyleSheet(ConfTWss);

    QTabWidget *measure = new QTabWidget;
    measure->tabBar()->setStyleSheet(ConfTWss);
    hlyout = new QHBoxLayout;
    vlyout = new QVBoxLayout;
    hlyout->addWidget(WDFunc::NewPB(this, "gj." + QString::number(Journals::JOURMEAS), "Получить журнал измерений", this, SLOT(TryGetJourByUSB())));
    hlyout->addWidget(WDFunc::NewPB(this, "ej." + QString::number(Journals::JOURMEAS), "Стереть журнал измерений", this, SLOT(EraseJour())));
    hlyout->addWidget(WDFunc::NewPB(this, "sj." + QString::number(Journals::JOURMEAS), "Сохранить журнал в файл", this, SLOT(SaveJour())));
    vlyout->addLayout(hlyout);
    vlyout->addWidget(WDFunc::NewTV(this, "meas", nullptr), 89);
    measure->setLayout(vlyout);
    measure->setStyleSheet(ConfTWss);
*/
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw4");
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(JourTab(Journals::JOURWORK), "Рабочий журнал");
    ConfTW->addTab(JourTab(Journals::JOURSYS),"Системный журнал");
    ConfTW->addTab(JourTab(Journals::JOURMEAS),"Журнал измерений");

    lyout->addWidget(ConfTW);
    setLayout(lyout);
    if (StdFunc::IsInEmulateMode())
    {
        WDFunc::SetEnabled(this, "gj." + QString::number(Journals::JOURSYS), false);
        WDFunc::SetEnabled(this, "gj." + QString::number(Journals::JOURMEAS), false);
        WDFunc::SetEnabled(this, "gj." + QString::number(Journals::JOURWORK), false);
        WDFunc::SetEnabled(this, "ej." + QString::number(Journals::JOURSYS), false);
        WDFunc::SetEnabled(this, "ej." + QString::number(Journals::JOURMEAS), false);
        WDFunc::SetEnabled(this, "ej." + QString::number(Journals::JOURWORK), false);
        WDFunc::SetEnabled(this, "sj." + QString::number(Journals::JOURSYS), false);
        WDFunc::SetEnabled(this, "sj." + QString::number(Journals::JOURMEAS), false);
        WDFunc::SetEnabled(this, "sj." + QString::number(Journals::JOURWORK), false);
    }
}

QWidget *JournalDialog::JourTab(int jourtype)
{
    QSortFilterProxyModel *mdl;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QWidget *w = new QWidget;
    QString str, tvname;

    switch(jourtype)
    {
    case Journals::JOURWORK:
        str = "рабочий журнал";
        tvname = "work";
        mdl = ProxyWorkModel;
        break;
    case Journals::JOURSYS:
        str = "системный журнал";
        tvname = "system";
        mdl = ProxySysModel;
        break;
    case Journals::JOURMEAS:
        str = "журнал измерений";
        tvname = "meas";
        mdl = ProxyMeasModel;
        break;
    }

    hlyout->addWidget(WDFunc::NewPB(this, "gj." + QString::number(jourtype), "Получить " + str, this, SLOT(TryGetJourByUSB())));
    hlyout->addWidget(WDFunc::NewPB(this, "ej." + QString::number(jourtype), "Стереть " + str, this, SLOT(EraseJour())));
    hlyout->addWidget(WDFunc::NewPB(this, "sj." + QString::number(jourtype), "Сохранить журнал в файл", this, SLOT(SaveJour())));
    vlyout->addLayout(hlyout);
    vlyout->addWidget(WDFunc::NewTV(this, tvname, mdl), 89);
    w->setLayout(vlyout);
    return w;
}

void JournalDialog::TryGetJourByUSB()
{
    QString filetofind;
    int jourtype = GetJourNum(sender()->objectName());
    if (jourtype == GENERALERROR)
    {
        ERMSG("Incorrect jour type");
        return;
    }
    if (jourtype == Journals::JOURSYS)
        filetofind = "system.dat";
    else if (jourtype == Journals::JOURWORK)
        filetofind = "workj.dat";
    else if (jourtype == Journals::JOURMEAS)
        filetofind = "measj.dat";
    else
    {
        ERMSG("Incorrect jour type");
        return;
    }
    JourType = jourtype;
    JourFuncs->SetJourType(jourtype);
    QByteArray ba;
    QStringList drives = Files::Drives();
    if (!drives.isEmpty())
    {
        QStringList files = Files::SearchForFile(drives, filetofind);
        if (!files.isEmpty())
        {
            JourFile = Files::GetFirstDriveWithLabel(files, "AVM");
            JourFuncs->SetJourFile(JourFile);
            if (JourFile.isEmpty())
                GetJour();
            else
                StartReadJourFile();
        }
        else
            GetJour();
    }
    else
        GetJour();
}

void JournalDialog::GetJour()
{
    emit StartGetJour();
}

void JournalDialog::JourFileChoosed(QString &file)
{
    JourFile = file;
}

void JournalDialog::EraseJour()
{
    if (MainInterface == I_USB)
    {
        if(WriteCheckPassword() == NOERROR)
        {
            int jourtype = GetJourNum(sender()->objectName());
            if (jourtype == GENERALERROR)
            {
                ERMSG("Ошибочный тип журнала");
                return;
            }
            char num = jourtype + 4;

            if(Commands::EraseTechBlock(num) == NOERROR)
            {
              EMessageBox::information(this, "Успешно", "Стирание прошло успешно");
            }
            else
            {
              EMessageBox::information(this, "Ошибка", "Ошибка");
            }
        }
    }
}

void JournalDialog::SaveJour()
{
    QString jourfilestr;
    QString tvname;
    int jtype = GetJourNum(sender()->objectName());
    if (jtype == GENERALERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибочный тип журнала");
        return;
    }

    if (jtype == Journals::JOURSYS)
    {
        tvname = "system";
        jourfilestr += "SysJ ";
    }
    else if (jtype == Journals::JOURWORK)
    {
        tvname = "work";
        jourfilestr += "WorkJ ";
    }
    else
    {
        tvname = "meas";
        jourfilestr += "MeasJ ";
    }

    QAbstractItemModel *amdl = WDFunc::TVModel(this, tvname);
    if (amdl == nullptr)
    {
        EMessageBox::error(this, "Ошибка", "Данные ещё не получены");
        return;
    }
    jourfilestr += QString::number(MTypeB, 16) + QString::number(MTypeM, 16) + " #" + QString("%1").arg(ModuleBSI::SerialNum(BoardTypes::BT_MODULE), 8, 10, QChar('0')) + " ";
    jourfilestr += QDate::currentDate().toString("dd-MM-yyyy") + ".xlsx";
    // запрашиваем имя файла для сохранения
    QString filename = Files::ChooseFileForSave(nullptr, "Excel documents (*.xlsx)", "xlsx", jourfilestr);
    WW = new WaitWidget;
    WW->SetMessage("Запись файла...");
    WW->Start();
    emit StartSaveJour(jtype, amdl, filename);
}

int JournalDialog::GetJourNum(const QString &objname)
{
    bool ok;
    QStringList sl = objname.split(".");

    if (sl.size() < 2)
    {
        DBGMSG;
        return GENERALERROR;
    }
    int jourtype = sl.at(1).toInt(&ok);
    QString jourprefix = sl.at(0);
    if (((jourprefix != "gj") && (jourprefix != "ej") && (jourprefix != "sj") && (jourprefix != "mj")) || !ok)
    {
        DBGMSG;
        return GENERALERROR;
    }
    return jourtype;
}

int JournalDialog::WriteCheckPassword()
{
    ok = false;
    StdFunc::ClearCancel();
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
    connect(dlg,SIGNAL(Finished(QString)),this,SLOT(WritePasswordCheck(QString)));
    connect(this,SIGNAL(WritePasswordChecked()),&PasswordLoop,SLOT(quit()));
    dlg->show();
    PasswordLoop.exec();
    if (StdFunc::IsCancelled())
        return GENERALERROR;
    if(!ok)
    {
        EMessageBox::error(this, "Неправильно", "Пароль введён неверно");
        return GENERALERROR;
    }
    return NOERROR;
}

void JournalDialog::StartReadJourFile()
{
//    QApplication::setOverrideCursor(Qt::WaitCursor);
    // Крутилка
    WW = new WaitWidget(nullptr);
    WW->SetMessage("Чтение файла..");
    WW->Start();
    emit StartReadFile();
}

void JournalDialog::WritePasswordCheck(QString psw)
{
    if (psw == "121941")
        ok = true;
    else
        ok = false;
    emit WritePasswordChecked();
}

void JournalDialog::Done(QString msg)
{
    if (WW != nullptr)
        WW->Stop();
//    QApplication::restoreOverrideCursor();
    EMessageBox::information(this, "Успешно", msg);
}

void JournalDialog::Error(QString msg)
{
    if (WW != nullptr)
        WW->Stop();
    ERMSG(msg);
//    QApplication::restoreOverrideCursor();
    EMessageBox::error(this, "Ошибка", msg);
}

void JournalDialog::SetModel(ETableModel *mdl)
{
    WW->SetMessage("Сортировка...");
    QSortFilterProxyModel *pmdl = new QSortFilterProxyModel;
    pmdl->setSourceModel(mdl);
    int dateidx = mdl->Headers().indexOf("Дата/Время UTC");
    switch(JourType)
    {
    case Journals::JOURWORK:
        WDFunc::SetTVModel(this, "work", pmdl, true);
        WDFunc::SortTV(this, "work", dateidx, Qt::DescendingOrder);
        break;
    case Journals::JOURSYS:
        WDFunc::SetTVModel(this, "system", pmdl, true);
        WDFunc::SortTV(this, "system", dateidx, Qt::DescendingOrder);
        break;
    case Journals::JOURMEAS:
        WDFunc::SetTVModel(this, "meas", pmdl, true);
        WDFunc::SortTV(this, "meas", dateidx, Qt::AscendingOrder);
        break;
    }
    Done("Прочитано успешно");
}

