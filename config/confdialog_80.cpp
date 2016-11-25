#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStringListModel>
#include <QSpinBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QMessageBox>
#include <QCoreApplication>
#include <QFileDialog>
#include "../canal.h"
#include "../widgets/messagebox.h"
#include "../widgets/s_tqcombobox.h"
#include "../widgets/s_tqcheckbox.h"
#include "../widgets/s_tqspinbox.h"
#include "confdialog_80.h"

confdialog_80::confdialog_80(QWidget *parent) :
    QDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    econf = new config_80;
    GeneralConf = new confdialog(&(econf->Bci_block.mainblk));
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    lyout->addWidget(ConfTW);
    QWidget *wdgt = new QWidget;
    QGridLayout *wdgtlyout = new QGridLayout;
    QPushButton *pb1 = new QPushButton("Прочитать из модуля");
    connect(pb1,SIGNAL(clicked()),this,SLOT(GetBci()));
    if (pc.Emul)
        pb1->setEnabled(false);
    wdgtlyout->addWidget(pb1, 0, 0, 1, 1);
    QPushButton *pb = new QPushButton("Записать в модуль");
    pb->setObjectName("WriteConfPB");
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteConfDataToModule()));
    if (pc.Emul)
        pb->setEnabled(false);
    wdgtlyout->addWidget(pb, 0, 1, 1, 1);
    pb = new QPushButton("Прочитать из файла");
    pb->setIcon(QIcon(":/load.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadConf()));
    wdgtlyout->addWidget(pb, 0, 2, 1, 1);
    pb = new QPushButton("Записать в файл");
    pb->setIcon(QIcon(":/save.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveConf()));
    wdgtlyout->addWidget(pb, 0, 3, 1, 1);
    pb = new QPushButton("Задать конфигурацию по умолчанию");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetDefConf()));
    wdgtlyout->addWidget(pb, 1, 0, 1, 2);
    pb = new QPushButton("Перейти на новую конфигурацию");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetNewConf()));
    if (pc.Emul)
        pb->setEnabled(false);
    wdgtlyout->addWidget(pb, 1, 2, 1, 2);
    wdgt->setLayout(wdgtlyout);
    lyout->addWidget(wdgt);
    setLayout(lyout);
    SetupUI();
    if ((pc.ModuleBsi.Hth & HTH_CONFIG) || (pc.Emul)) // если установлен признак отсутствия конфигурации
        SetDefConf();
    else
        GetBci();
}

confdialog_80::~confdialog_80()
{
    delete GeneralConf;
}

void confdialog_80::GetBci()
{
    cn->Send(CN_GF,NULL,0,1,econf->Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
    {
        FillConfData();
        MessageBox2::information(this, "OK", "Прочитано успешно");
    }
}

void confdialog_80::FillConfData()
{
    int i;
    QSpinBox *spb;
    s_tqSpinBox *dspbls;
    s_tqComboBox *cb;
    s_tqCheckBox *chb;

    GeneralConf->Fill(); // заполнение общих полей
    SetComboBox("eq_typecb", econf->Bci_block.eq_type);
    SetCheckBox("osc1chb", (econf->Bci_block.ddosc & 0x0001));
    SetCheckBox("osc2chb", (econf->Bci_block.ddosc & 0x0002));
    SetCheckBox("osc3chb", (econf->Bci_block.ddosc & 0x0004));
    SetSpinBox("npointspb", econf->Bci_block.npoints);
    SetSpinBox("nhfiltrspb", econf->Bci_block.nhfiltr);
    switch (pc.ModuleBsi.MTypeM)
    {
    case MTM_81: // 2 напряжения, 0 токов
    {
        SetComboBox("unom.1", QString::number(econf->Bci_block.unom1));
        SetComboBox("unom.2", QString::number(econf->Bci_block.unom2));
        SetSpinBox("thr.1", econf->Bci_block.duosc);
       break;
    }
    case MTM_82:
    {
        SetComboBox("unom.1", QString::number(econf->Bci_block.unom1));
        for (i = 3; i < 6; i++)
            SetSpinBox("inom.2."+QString::number(i), econf->Bci_block.inom2[i]);
        SetSpinBox("thr.1", econf->Bci_block.duosc);
        SetSpinBox("thr.2", econf->Bci_block.diosc);
        break;
    }
    case MTM_83:
    {
        for (i = 0; i < 6; i++)
        {
            SetSpinBox("inom.1."+QString::number(i), econf->Bci_block.inom1[i]);
            SetSpinBox("inom.2."+QString::number(i), econf->Bci_block.inom2[i]);
        }
        SetSpinBox("thr.2", econf->Bci_block.diosc);
       break;
    }
    default:
        break;
    }
    SetSpinBox("thr.3", econf->Bci_block.duimin);
}

void confdialog_80::SetupUI()
{
    int i;
    QGridLayout *lyout1 = new QGridLayout;
//    QGridLayout *lyout2 = new QGridLayout;
//    QGridLayout *lyout3 = new QGridLayout;
    QGridLayout *lyout4 = new QGridLayout;
    QWidget *cp1 = new QWidget;
//    QWidget *cp2 = new QWidget;
//    QWidget *cp3 = new QWidget;
    QWidget *cp4 = new QWidget;
    QString tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
//    cp2->setStyleSheet(tmps);
//    cp3->setStyleSheet(tmps);
    cp4->setStyleSheet(tmps);
    QTabWidget *ConfTW = this->findChild<QTabWidget *>("conftw");
    if (ConfTW == 0)
        return;
    ConfTW->addTab(cp1,"Общие");
    ConfTW->addTab(cp4,"104");
    QGroupBox *gb = new QGroupBox;
    QVBoxLayout *gblyout = new QVBoxLayout;
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QGridLayout *gb3lyout = new QGridLayout;
    QLabel *lbl = new QLabel;
    s_tqComboBox *cb = new s_tqComboBox;
    QStringList cbl;
    QStringListModel *cblm = new QStringListModel;
    QSpinBox *spb = new QSpinBox;
    s_tqCheckBox *chb;

    lyout4->addWidget(GeneralConf->Widget104());
    cp4->setLayout(lyout4);

    lbl = new QLabel ("Тип контролируемого оборудования:");
    lyout1->addWidget(lbl,0,0,1,1);
    cbl = QStringList() << "1ф трансформатор/АТ" << "3ф трансформатор/АТ" << "1ф реактор" << "3ф реактор";
    cblm = new QStringListModel;
    cblm->setStringList(cbl);
    cb = new s_tqComboBox;
    cb->setObjectName("eq_typecb");
    cb->setModel(cblm);
    cb->setMinimumWidth(70);
    tmps = "QComboBox {background-color: "+QString(ACONFGCLR)+";}";
    cb->setStyleSheet(tmps);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetEqType(int)));
    lyout1->addWidget(cb,0,1,1,1);

    gb = new QGroupBox("Аналоговые");
    gblyout = new QVBoxLayout;
    switch (pc.ModuleBsi.MTypeM)
    {
    case MTM_81: // 6U0I
    {
        gblyout->addWidget(UNom(1));
        gblyout->addWidget(UNom(2));
        gblyout->addWidget(Threshold("Уставка скачка напряжения для запуска осциллографирования, %", 1));
        break;
    }
    case MTM_82: // 3U3I
    {
        gblyout->addWidget(UNom(1));
        gblyout->addWidget(INom(2));

        lbl=new QLabel("Номинальные вторичные токи ТТ 2-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        cbl = QStringList() << "1" << "5";
        cblm = new QStringListModel;
        cblm->setStringList(cbl);
        for (i = 3; i < 6; i++)
        {
            lbl=new QLabel(QString::number(i+7, 16).toUpper() + ":"); // A, B, C
            gb2lyout->setAlignment(lbl, Qt::AlignRight);
            gb2lyout->addWidget(lbl);
            cb = new s_tqComboBox;
            cb->setObjectName("inom2"+QString::number(i));
            cb->setModel(cblm);
            cb->setAData(i);
            connect(cb,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetSecCurrent(int,s_tqComboBox*)));
            gb2lyout->addWidget(cb);
        }
        gblyout->addLayout(gb2lyout);

        gblyout->addWidget(Threshold("Уставка скачка напряжения для запуска осциллографирования, %", 1));
        gblyout->addWidget(Threshold("Уставка скачка тока для запуска осциллографирования, %", 2));

        break;
    }
    case MTM_83: // 0U6I
    {
        lbl=new QLabel("Номинальные первичные токи ТТ 1-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        for (i = 0; i < 3; i++)
        {
            lbl=new QLabel(QString::number(i+10, 16).toUpper() + ":"); // A, B, C
            gb2lyout->addWidget(lbl);
            dspbls = new s_tqSpinBox;
            dspbls->setObjectName("inom1"+QString::number(i));
            dspbls->setSingleStep(1);
            dspbls->setMinimum(1);
            dspbls->setMaximum(50000);
            dspbls->setDecimals(0);
            dspbls->setAData(i);
            connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetCurrent(double,s_tqSpinBox*)));
            gb2lyout->addWidget(dspbls);
        }
        gblyout->addLayout(gb2lyout);

        lbl=new QLabel("Номинальные вторичные токи ТТ 1-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        cbl = QStringList() << "1" << "5";
        cblm = new QStringListModel;
        cblm->setStringList(cbl);
        for (i = 0; i < 3; i++)
        {
            lbl=new QLabel(QString::number(i+10, 16).toUpper() + ":"); // A, B, C
            gb2lyout->addWidget(lbl);
            cb = new s_tqComboBox;
            cb->setObjectName("inom2"+QString::number(i));
            cb->setModel(cblm);
            cb->setAData(i);
            connect(cb,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetSecCurrent(int,s_tqComboBox*)));
            gb2lyout->addWidget(cb);
        }
        gblyout->addLayout(gb2lyout);

        lbl=new QLabel("Номинальные первичные токи ТТ 2-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        for (i = 3; i < 6; i++)
        {
            lbl=new QLabel(QString::number(i+7, 16).toUpper() + ":"); // A, B, C
            gb2lyout->addWidget(lbl);
            dspbls = new s_tqSpinBox;
            dspbls->setObjectName("inom1"+QString::number(i));
            dspbls->setSingleStep(1);
            dspbls->setMinimum(1);
            dspbls->setMaximum(50000);
            dspbls->setDecimals(0);
            dspbls->setAData(i);
            connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetCurrent(double,s_tqSpinBox*)));
            gb2lyout->addWidget(dspbls);
        }
        gblyout->addLayout(gb2lyout);

        lbl=new QLabel("Номинальные вторичные токи ТТ 2-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        cbl = QStringList() << "1" << "5";
        cblm = new QStringListModel;
        cblm->setStringList(cbl);
        for (i = 3; i < 6; i++)
        {
            lbl=new QLabel(QString::number(i+7, 16).toUpper() + ":"); // A, B, C
            gb2lyout->addWidget(lbl);
            cb = new s_tqComboBox;
            cb->setObjectName("inom2"+QString::number(i));
            cb->setModel(cblm);
            cb->setAData(i);
            connect(cb,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetSecCurrent(int,s_tqComboBox*)));
            gb2lyout->addWidget(cb);
        }
        gblyout->addLayout(gb2lyout);

        gblyout->addWidget(Threshold("Уставка скачка тока для запуска осциллографирования, %", 2));
        break;
    }
    default:
        break;
    }
    gblyout->addWidget(Threshold("Уставка порога мин. уровня для определения частоты, %", 3));
    gb->setLayout(gblyout);
    lyout1->addWidget(gb,1,0,1,2);

    gb = new QGroupBox("Осциллограммы");
    gblyout = new QVBoxLayout;
    gb2lyout = new QHBoxLayout;
    lbl = new QLabel("Запуск осциллограммы:");
    gb2lyout->addWidget(lbl);
    chb = new s_tqCheckBox;
    chb->setObjectName("osc1chb");
    chb->setText("по команде Ц");
    chb->setAData(0);
    connect(chb,SIGNAL(statechanged(int,s_tqCheckBox*)),this,SLOT(SetOsc(int,s_tqCheckBox*)));
    gb2lyout->addWidget(chb);
    chb = new s_tqCheckBox;
    chb->setObjectName("osc2chb");
    chb->setText("по дискр. входу PD1");
    chb->setAData(1);
    connect(chb,SIGNAL(statechanged(int,s_tqCheckBox*)),this,SLOT(SetOsc(int,s_tqCheckBox*)));
    gb2lyout->addWidget(chb);
    chb = new s_tqCheckBox;
    chb->setObjectName("osc3chb");
    chb->setText("по резкому изменению");
    chb->setAData(2);
    connect(chb,SIGNAL(statechanged(int,s_tqCheckBox*)),this,SLOT(SetOsc(int,s_tqCheckBox*)));
    gb2lyout->addWidget(chb);
    gblyout->addLayout(gb2lyout);
    gb->setLayout(gblyout);
    lyout1->addWidget(gb,2,0,1,2);

    gb = new QGroupBox("Прочие");
    gb3lyout = new QGridLayout;
    lbl = new QLabel("Кол-во точек оцифровки:");
    gb3lyout->addWidget(lbl, 0, 0, 1, 1);
    cbl = QStringList() << "64" << "80" << "128" << "256";
    cblm = new QStringListModel;
    cblm->setStringList(cbl);
    cb = new s_tqComboBox;
    cb->setObjectName("npointscb");
    cb->setModel(cblm);
    connect(cb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetNPoints(QString)));
    gb3lyout->addWidget(cb,0,1,1,1);
    lbl = new QLabel("Постоянная времени фильтрации:");
    gb3lyout->addWidget(lbl, 1, 0, 1, 1);
    spb = new QSpinBox;
    spb->setObjectName("nfiltrspb");
    spb->setMinimum(1);
    spb->setMaximum(1000);
    connect(spb,SIGNAL(valueChanged(int)),this,SLOT(SetNFiltr(int)));
    gb3lyout->addWidget(spb,1,1,1,1);
    lbl = new QLabel("Постоянная времени гармоник:");
    gb3lyout->addWidget(lbl, 2, 0, 1, 1);
    spb = new QSpinBox;
    spb->setObjectName("nhfiltrspb");
    spb->setMinimum(1);
    spb->setMaximum(10);
    connect(spb,SIGNAL(valueChanged(int)),this,SLOT(SetNHFiltr(int)));
    gb3lyout->addWidget(spb,2,1,1,1);
    gb->setLayout(gb3lyout);
    lyout1->addWidget(gb,3,0,1,2);

    cp1->setLayout(lyout1);
//    cp2->setLayout(lyout2);
//    cp3->setLayout(lyout3);
}

QWidget *confdialog_80::UNom(int numunom)
{
    QWidget *w = new QWidget;
    QString NumUNomStr = QString::number(numunom);
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    lbl=new QLabel("Класс напряжения "+NumUNomStr+"-й группы, кВ:");
    gb2lyout->addWidget(lbl);
    cbl = QStringList() << "1150" << "750" << "500" << "330" << "220" << "110" << "35" << "21" << "15.75" << "11" << "10" << "6.3";
    cblm = new QStringListModel;
    cblm->setStringList(cbl);
    cb = new s_tqComboBox;
    cb->setObjectName("unom."+NumUNomStr);
    cb->setModel(cblm);
    cb->setEditable(true);
    tmps = "QComboBox {background-color: "+QString(ACONFGCLR)+";}";
    cb->setStyleSheet(tmps);
    connect(cb,SIGNAL(textChanged(int)),this,SLOT(SetVoltageClass(int)));
    gb2lyout->addWidget(cb);
    w->setLayout(gb2lyout);
    return w;
}

QWidget *confdialog_80::INom(int numinom)
{
    QWidget *w = new QWidget;
    QVBoxLayout *gblyout = new QVBoxLayout;
    QString NumGroup = (numinom < 3) ? "1" : "2";
    QString Perv = (numinom%2) ? "первичные" : "вторичные";
    QString PervNum = (numinom%2) ? "1" : "2";
    QLabel *lbl=new QLabel("Номинальные "+Perv+" токи ТТ "+NumGroup+"-й группы, А:");
    gblyout->addWidget(lbl);
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    for (i = 0; i < 3; i++)
    {
        lbl=new QLabel(QString::number(i+10, 16).toUpper() + ":"); // A, B, C
        gb2lyout->addWidget(lbl, Qt::AlignRight);
        dspbls = new s_tqSpinBox;
        dspbls->setObjectName("inom."+NumGroup+"."+PervNum+"."+QString::number(i)); // inom.<группа 1|2>.<тип перв/втор 1|2>.<phase>
        dspbls->setSingleStep(1);
        dspbls->setMinimum(1);
        dspbls->setMaximum(50000);
        dspbls->setDecimals(0);
        connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(SetCurrent(double)));
        gb2lyout->addWidget(dspbls);
    }
    gblyout->addLayout(gb2lyout);
}

QWidget *confdialog_80::Threshold(QString &str, int numthr)
{
    QWidget *w = new QWidget;
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    lbl = new QLabel(str);
    gb2lyout->addWidget(lbl);
    dspbls = new s_tqSpinBox;
    dspbls->setObjectName("thr."+QString::number(numthr));
    dspbls->setSingleStep(0.1);
    dspbls->setMinimum(0.0);
    dspbls->setMaximum(100.0);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(SetThreshold(double)));
    gb2lyout->addWidget(dspbls);
    w->setLayout(gb2lyout);
}

void confdialog_80::SetOsc(int isChecked)
{
    quint16 tmpint = 0x0001;
    tmpint = tmpint << ptr->getAData().toInt();
    if (isChecked == Qt::Checked)
        econf->Bci_block.ddosc |= tmpint;
    else
        econf->Bci_block.ddosc &= ~tmpint;
}

void confdialog_80::SetThreshold(double dbl)
{
    int thr;
    if ((thr = GetWNum(sender()->objectName()) == 0))
    {
        DBGMSG;
        return;
    }
    switch (thr)
    {
    case 1: // % напряжения
    {
        econf->Bci_block.duosc = dbl;
        break;
    }
    case 2: // % тока
    {
        econf->Bci_block.diosc = dbl;
        break;
    }
    case 3: // % частоты
    {
        econf->Bci_block.duimin = dbl;
        break;
    }
    default:
        break;
    }
}

void confdialog_80::SetVoltageClass(int tmpi)
{
    int vnum;
    if ((vnum = GetWNum(sender()->objectName()) == 0))
    {
        DBGMSG;
        return;
    }
    if (vnum == 2) // вторая тройка напряжений
        econf->Bci_block.unom2 = tmpi;
    else
        econf->Bci_block.unom1 = tmpi;
}

// по имени виджета взять его номер

int confdialog_80::GetWNum(QString &ObjName)
{
    QStringList sl = sender().objectName.split(".");
    if (sl.size() < 1)
    {
        DBGMSG;
        return 0;
    }
    bool ok;
    int vnum = sl.at(1).toInt(&ok);
    if (!ok)
    {
        DBGMSG;
        return 0;
    }
    return vnum;
}

void confdialog_80::SetCurrent(double dbl)
{
    QStringList sl = sender()->objectName().split(".");
    if (sl.size() < 4) // name, group, perv, phase
    {
        DBGMSG;
        return;
    }
    bool ok;
    int group, perv, phase;
    group = sl.at(1).toInt(&ok);
    if (ok)
    {
        perv = sl.at(2).toInt(&ok);
        if (ok)
        {
            phase = sl.at(3).toInt(&ok);
            if (ok)
            {
                if (perv == 1) // первичная группа
                    econf->Bci_block.inom1[(group-1)*3+phase] = dbl;
                else
                    econf->Bci_block.inom2[(group-1)*3+phase] = dbl;
                return;
            }
        }
    }
    DBGMSG;
}

void confdialog_80::SetEqType(int tmpi)
{
    econf->Bci_block.eq_type = tmpi;
}

void confdialog_80::SetNPoints(QString tmpi)
{
    econf->Bci_block.npoints = tmpi.toInt();
}

void confdialog_80::SetNFiltr(int tmpi)
{
    econf->Bci_block.nfiltr = tmpi;
}

void confdialog_80::SetNHFiltr(int tmpi)
{
    econf->Bci_block.nhfiltr = tmpi;
}

void confdialog_80::WriteConfDataToModule()
{
    cn->Send(CN_WF, &econf->Bci_block, sizeof(econf->Bci_block), 2, econf->Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        MessageBox2::information(this, "OK", "Операция проведена успешно!");
}

void confdialog_80::SetNewConf()
{
    cn->Send(CN_Cnc);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
    {
        MessageBox2::information(this, "OK", "Переведено успешно");
        emit BsiIsNeedToBeAcquiredAndChecked();
    }
}

void confdialog_80::SetDefConf()
{
    econf->Bci_defblock.mainblk.MTypeB = pc.ModuleBsi.MTypeB; // делаем для того, чтобы типы совпадали
    econf->Bci_defblock.mainblk.MTypeM = pc.ModuleBsi.MTypeM; // делаем для того, чтобы подтипы совпадали
    memcpy(&econf->Bci_block, &econf->Bci_defblock, sizeof(config_80::Bci));
    FillConfData();
}

void confdialog_80::LoadConf()
{
    QByteArray ba;
    ba = pc.LoadFile("Config files (*.ecf)");
    if (pc.RestoreDataMem(&(ba.data()[0]), ba.size(), econf->Config))
    {
        WARNMSG("");
        return;
    }
    FillConfData();
    MessageBox2::information(this, "OK", "Загрузка прошла успешно!");
}

void confdialog_80::SaveConf()
{
    QByteArray *ba = new QByteArray;
    ba->resize(MAXBYTEARRAY);
    pc.StoreDataMem(&(ba->data()[0]), econf->Config);
    quint32 BaLength = static_cast<quint8>(ba->data()[0]);
    BaLength += static_cast<quint8>(ba->data()[1])*256;
    BaLength += static_cast<quint8>(ba->data()[2])*65536;
    BaLength += static_cast<quint8>(ba->data()[3])*16777216;
    BaLength += 12; // FileHeader
    int res = pc.SaveFile("Config files (*.ecf)", &(ba->data()[0]), BaLength);
    switch (res)
    {
    case 0:
        MessageBox2::information(this, "OK", "Записано успешно!");
        break;
    case 1:
        MessageBox2::error(this, "error", "Ошибка при записи файла!");
        break;
    case 2:
        MessageBox2::error(this, "error", "Пустое имя файла!");
        break;
    case 3:
        MessageBox2::error(this, "error", "Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void confdialog_80::SetComboBox(QString &name, QString &text)
{
    s_tqComboBox *cb = this->findChild<s_tqComboBox *>(name);
    if (cb != 0)
        cb->setCurrentText(text);
}

void confdialog_80::SetSpinBox(QString &name, double value)
{
    s_tqSpinBox *dspbls = this->findChild<s_tqSpinBox *>(name);
    if (dspbls != 0)
        dspbls->setValue(value);
}

void confdialog_80::SetCheckBox(QString &name, bool isChecked)
{
    s_tqCheckBox *chb = this->findChild<s_tqCheckBox *>(name);
    if (chb != 0)
        chb->setChecked(isChecked);
}
