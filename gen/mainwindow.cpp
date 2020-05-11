#include <QHBoxLayout>
#include <QTextEdit>
//#include <QPushButton>
#include <QDir>
#include <QMenu>
#include <QApplication>
#include <QTimer>
#include <QMenuBar>
#include <QEventLoop>
#include <QScrollBar>
#include <QProgressBar>
#include <QSettings>
#include <QGroupBox>
#include <QCursor>
#include <QStringListModel>
#include <QStandardPaths>
#include <QPropertyAnimation>
#include <QtSerialPort/QSerialPortInfo>
#include <QFileDialog>
#include "mainwindow.h"
#include "../widgets/wd_func.h"
#include "../widgets/etabwidget.h"
#include "../widgets/emessagebox.h"
#include "../dialogs/errordialog.h"
#include "../dialogs/hiddendialog.h"
#include "../dialogs/settingsdialog.h"
#include "../dialogs/keypressdialog.h"
#include "../gen/error.h"
#include "../gen/colors.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../widgets/etablemodel.h"
#include "../widgets/etableview.h"
#include "../gen/timefunc.h"
#include "modulebsi.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "commands.h"
#endif

QString MainWindow::MainInterface;
quint32 MainWindow::MTypeB;
quint32 MainWindow::MTypeM;
int MainWindow::TheEnd;
int MainWindow::StopRead;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QPixmap StartWindowSplashPixmap("images/2.1.x.png");
    StartWindowSplashScreen = new QSplashScreen(StartWindowSplashPixmap);
    StartWindowSplashScreen->show();
    StartWindowSplashScreen->showMessage("Подготовка окружения...", Qt::AlignRight, Qt::white);
    // http://stackoverflow.com/questions/2241808/checking-if-a-folder-exists-and-creating-folders-in-qt-c
    QDir dir(StdFunc::GetHomeDir());
    if (!dir.exists())
        dir.mkpath(".");
    StdFunc::Init();
    Error::Init();
    S2Config.clear();
    S2ConfigForTune.clear();
    MainConfDialog = nullptr;
    ConfB = ConfM = nullptr;
    CheckB = CheckM = nullptr;
    ch104 = nullptr;
    //iec104* SaveCh104 = new iec104("172.16.28.5", this);
    //ModBusThrFinished = false;
    //TimeThrFinished = false;
    BdaTimer = new QTimer;
    BdaTimer->setInterval(ANMEASINT);

    ReceiveTimer = new QTimer;
    ReceiveTimer->setInterval(ANMEASINT);
    connect(ReceiveTimer, SIGNAL(timeout()), this,SLOT(FileTimeOut()));

    for (int i = 0; i < 20; ++i)
    {
       PredAlarmEvents[i] = 0;
       AlarmEvents[i] = 0;
    }
    TheEnd = 0;

//#endif

//#if PROGSIZE >= PROGSIZE_LARGE
    PrepareTimers();
//#endif
    LoadSettings();

    StartWindowSplashScreen->finish(this);

}

MainWindow::~MainWindow()
{
    //DisconnectAndClear();
#if PROGSIZE != PROGSIZE_EMUL
    Disconnect();
#endif
}

void MainWindow::SetMode(int mode)
{
    Mode = mode;
}

int MainWindow::GetMode()
{
    return Mode;
}

void MainWindow::Go(const QString &parameter)
{
    Q_UNUSED(parameter)
    if (Mode != COMA_GENERALMODE)
    {
        StdFunc::SetEmulated(true);
        Autonomous = true;
    }
    SetupUI();
    show();

}

QWidget *MainWindow::HthWidget()
{
    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");
    QHBoxLayout *hlyout = new QHBoxLayout;
    for (int i = (MAXERRORFLAGNUM-1); i >= 0; i--)
        hlyout->addWidget(WDFunc::NewLBLT(w, Hth().at(i), "hth"+QString::number(i), \
                                          "QLabel {background-color: rgba(255,50,50,0); color: rgba(220,220,220,255);" \
                                          "background: 0px; margin: 0px; spacing: 0; padding: 0px;}", HthToolTip().at(i)));
    w->setLayout(hlyout);
    connect(this,SIGNAL(BsiRefresh(ModuleBSI::Bsi*)), this, SLOT(UpdateHthWidget(ModuleBSI::Bsi*)));
    return w;
}

QWidget *MainWindow::ReleWidget()
{
    QMenu *menu = new QMenu;
    QString tmps = "QMenuBar {background-color: "+QString(MAINWINCLR)+";}"\
            "QMenuBar::item {background-color: "+QString(MAINWINCLR)+";}";
    menu->setStyleSheet(tmps);
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QHBoxLayout *hlyout2 = new QHBoxLayout;
    QWidget *w = new QWidget();
    QStringList Discription =  QStringList() << "Состояние устройства" << "Предупредительная сигнализация" << "Аварийная сигнализация";
    w->setStyleSheet("QMainWindow {background-color: "+QString(MAINWINCLR)+";}");
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QLabel lab;

    //for (int i = 0; i < 3; ++i)
    //{
        QAction *act = new QAction(this);
        act->setText("Состояние устройства");
        connect(act,SIGNAL(triggered()),this,SLOT(DeviceState()));
        menu->addAction(act);
        menu->setMinimumWidth(200);
        menu->setMinimumHeight(30);
        menu->popup(QCursor::pos());
        QGroupBox *gb = new QGroupBox("");

        //setMenuBar(menubar);
        //hlyout->addWidget(WDFunc::NewLBLT(w, "                 Реле №"+ QString::number(i+1) +": ", "", "", Discription.at(i)));
        hlyout->addWidget(menu,Qt::AlignRight);
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(950), pmgrn), 1);
        gb->setLayout(hlyout);
        hlyout2->addWidget(gb);
        /*if ((i>0)&&!((i+1)%8))
        {
            vlyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }*/
    //}
        gb = new QGroupBox("");
        hlyout = new QHBoxLayout;
        menu = new QMenu;
        act = new QAction(this);
        act->setText("Предупредительная сигнализация");
        connect(act,SIGNAL(triggered()),this,SLOT(PredAlarmState()));
        menu->addAction(act);
        menu->setMinimumWidth(250);
        menu->setMinimumHeight(30);
        menu->popup(QCursor::pos());

        hlyout->addWidget(menu,Qt::AlignRight);
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(951), pmgrn), 1);
        gb->setLayout(hlyout);
        hlyout2->addWidget(gb);

        menu = new QMenu;
        gb = new QGroupBox("");
        hlyout = new QHBoxLayout;
        act = new QAction(this);
        act->setText("Аварийная сигнализация");
        connect(act,SIGNAL(triggered()),this,SLOT(AlarmState()));
        menu->addAction(act);
        menu->setMinimumWidth(200);
        menu->setMinimumHeight(30);
        menu->popup(QCursor::pos());

       // menubar->addSeparator();
        hlyout->addWidget(menu,Qt::AlignRight);
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(952), pmgrn), 1);
        gb->setLayout(hlyout);
        hlyout2->addWidget(gb);



    if (hlyout2->count())
        vlyout->addLayout(hlyout2);
    w->setLayout(vlyout);
    return w;
}

void MainWindow::DeviceState()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    QString tmps = QString(PROGCAPTION);

    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    //QPixmap *pm[2] = {pmred, pmgrn};                        ";


    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");  // color: rgba(220,220,220,255);

    for (int i = 0; i < HthToolTip().size(); ++i)
    {
        hlyout = new QHBoxLayout;

        if(ModuleBSI::ModuleBsi.Hth & (0x00000001<<i))
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(i), pmred));
        else
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(i), pmgrn));

        hlyout->addWidget(WDFunc::NewLBLT(w, HthToolTip().at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    w->setLayout(vlayout);
    connect(this,SIGNAL(BsiRefresh(ModuleBSI::Bsi*)), this, SLOT(UpdateHthWidget(ModuleBSI::Bsi*)));

    //hlyout->addLayout(l2yout,100);
    lyout->addWidget(w);
/*    QPushButton *pb = new QPushButton("Ok");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb,0); */
    lyout->addWidget(WDFunc::NewPB(this, "", "Ok", dlg, SLOT(close)), 0);
    dlg->setLayout(lyout);
    dlg->exec();
}

void MainWindow::PredAlarmState()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    QString tmps = QString(PROGCAPTION);

    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    //QPixmap *pm[2] = {pmred, pmgrn};
    QStringList events = QStringList() << "Отсутствует сигнал напряжения фазы A                   "
                                       << "Отсутствует сигнал напряжения фазы B                   "
                                       << "Отсутствует сигнал напряжения фазы С                   "
                                       << "Отсутствует ток ввода фазы А (ток меньше 2мА)          "
                                       << "Отсутствует ток ввода фазы B (ток меньше 2мА)          "
                                       << "Отсутствует ток ввода фазы C (ток меньше 2мА)          "
                                       << "Не заданы начальные значения                           "
                                       << "Низкое напряжение фазы A                               "
                                       << "Низкое напряжение фазы B                               "
                                       << "Низкое напряжение фазы C                               "
                                       << "Сигнализация по приращению тангенса дельта ввода фазы А"
                                       << "Сигнализация по приращению тангенса дельта ввода фазы B"
                                       << "Сигнализация по приращению тангенса дельта ввода фазы C"
                                       << "Сигнализация по приращению C ввода фазы А              "
                                       << "Сигнализация по приращению C ввода фазы B              "
                                       << "Сигнализация по приращению C ввода фазы C              "
                                       << "Не заданы паспортные значения                          ";


    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");  // color: rgba(220,220,220,255);

    for (int i = 0; i < 13; ++i)
    {
        hlyout = new QHBoxLayout;

        if(PredAlarmEvents[i])
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3011+i), pmred));
        else
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3011+i), pmgrn));

        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    for (int i = 0; i < 3; ++i)
    {
        hlyout = new QHBoxLayout;

        if(PredAlarmEvents[13+i])
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3027+i), pmred));
        else
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3027+i), pmgrn));

        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(13+i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    hlyout = new QHBoxLayout;
    if(PredAlarmEvents[16])
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3033), pmred));
    else
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3033), pmgrn));

    hlyout->addWidget(WDFunc::NewLBLT(w, events.at(16), "", "", ""), 1);
    vlayout->addLayout(hlyout);

    w->setLayout(vlayout);

    if(ch104 != nullptr)
    connect(ch104,SIGNAL(sponsignalsready(Parse104::SponSignals104*)), this, SLOT(UpdatePredAlarmEvents(Parse104::SponSignals104*)));

    //hlyout->addLayout(l2yout,100);
    lyout->addWidget(w);
/*    QPushButton *pb = new QPushButton("Ok");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb,0); */
    lyout->addWidget(WDFunc::NewPB(this, "", "Ok", dlg, SLOT(close)), 0);
    dlg->setLayout(lyout);
    dlg->exec();
}


void MainWindow::AlarmState()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    QString tmps = QString(PROGCAPTION);

    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    //QPixmap *pm[2] = {pmred, pmgrn};
    QStringList events = QStringList() << "Авария по приращению тангенса дельта ввода фазы А"
                                       << "Авария по приращению тангенса дельта ввода фазы B"
                                       << "Авария по приращению тангенса дельта ввода фазы C"
                                       << "Авария по приращению C ввода фазы А              "
                                       << "Авария по приращению C ввода фазы B              "
                                       << "Авария по приращению C ввода фазы C              ";

    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");  // color: rgba(220,220,220,255);

    for (int i = 0; i < 3; ++i)
    {
        hlyout = new QHBoxLayout;

        if(AlarmEvents[i])
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3024+i), pmred));
        else
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3024+i), pmgrn));

        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    for (int i = 0; i < 3; ++i)
    {
        hlyout = new QHBoxLayout;

        if(AlarmEvents[3+i])
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3030+i), pmred));
        else
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3030+i), pmgrn));

        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(3+i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    w->setLayout(vlayout);

    if(ch104 != nullptr)
    connect(ch104,SIGNAL(sponsignalsready(Parse104::SponSignals104*)), this, SLOT(UpdatePredAlarmEvents(Parse104::SponSignals104*)));

    //hlyout->addLayout(l2yout,100);
    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb,0);
    dlg->setLayout(lyout);
    dlg->exec();
}


void MainWindow::UpdateReleWidget(Parse104::SponSignals104* Signal)
{
//    int i = 0;
    //Parse104::SponSignals104 sig = *new Parse104::SponSignals104;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QPixmap *pm[2] = {pmred, pmgrn};

    //for(j=0; j<Signal->SigNumber; j++)
    //{
       //sig = *(Signal+j);
    if(!(Signal->Spon.SigVal & 0x80))
    {
       int signal = ((Signal->Spon.SigVal & (0x00000001)) ? 1 : 0);
       WDFunc::SetLBLImage(this, (QString::number(Signal->Spon.SigAdr)), pm[signal]);
    }
    //}

}

void MainWindow::UpdatePredAlarmEvents(Parse104::SponSignals104* Signal)
{
    int i = 0;
    Parse104::SponSignals104 sig = *new Parse104::SponSignals104;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QPixmap *pm[2] = {pmred, pmgrn};

    for(i=0; i<Signal->SigNumber; i++)
    {
       sig = *(Signal+i);
       if(!(sig.Spon.SigVal & 0x80))
       {
        int signal = ((sig.Spon.SigVal & (0x00000001)) ? 1 : 0);
        WDFunc::SetLBLImage(this, (QString::number(sig.Spon.SigAdr)), pm[signal]);
       }
    }

}


void MainWindow::UpdateStatePredAlarmEvents(Parse104::SponSignals104* Signal)
{
    int i = 0;
    Parse104::SponSignals104 sig = *new Parse104::SponSignals104;
//    QPixmap *pmgrn = new QPixmap("images/greenc.png");
//    QPixmap *pmred = new QPixmap("images/redc.png");
    //QPixmap *pm[2] = {pmred, pmgrn};

    for(i=0; i<Signal->SigNumber; i++)
    {
       sig = *(Signal+i);
       if(!(sig.Spon.SigVal & 0x80))
       {
           quint8 signal = ((sig.Spon.SigVal & (0x00000001)) ? 1 : 0);

           if((sig.Spon.SigAdr >= 3011) && (sig.Spon.SigAdr < 3024))
           PredAlarmEvents[sig.Spon.SigAdr - 3011] = signal;
           else if((sig.Spon.SigAdr >= 3024) && (sig.Spon.SigAdr < 3027))
           AlarmEvents[sig.Spon.SigAdr - 3024] = signal;
           else if((sig.Spon.SigAdr >= 3027) && (sig.Spon.SigAdr < 3030))
           PredAlarmEvents[sig.Spon.SigAdr - 3014] = signal;
           else if((sig.Spon.SigAdr >= 3030) && (sig.Spon.SigAdr < 3033))
           AlarmEvents[sig.Spon.SigAdr - 3027] = signal;
           else if(sig.Spon.SigAdr == 3033)
           PredAlarmEvents[16] = signal;
       }
    }

}

QWidget *MainWindow::Least()
{
    QWidget *w = new QWidget;
//    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *inlyout = new QHBoxLayout;
    ETabWidget *MainTW = new ETabWidget;
    MainTW->setObjectName("maintw");
    MainTW->setTabPosition(QTabWidget::West);
//    MainTW->tabBar()->setStyleSheet("QTabBar::tab {background-color: yellow;}");
    inlyout->addWidget(MainTW, 60);
    MainTW->hide();
    lyout->addLayout(inlyout, 90);
    lyout->addStretch(500);
    QFrame *line = new QFrame;
    line->setLineWidth(0);
    line->setMidLineWidth(1);
    line->setFrameStyle(QFrame::Sunken | QFrame::HLine);
    lyout->addWidget(line);
    inlyout = new QHBoxLayout;
    inlyout->addWidget(WDFunc::NewLBLT(this, "Обмен"));
    inlyout->addWidget(WDFunc::NewLBLT(this, "", "prb1lbl"));
    QProgressBar *prb = new QProgressBar;
    prb->setObjectName("prb1prb");
    prb->setOrientation(Qt::Horizontal);
    prb->setMinimumWidth(500);
    prb->setMaximumHeight(10);
    inlyout->addWidget(prb);
    lyout->addLayout(inlyout);
    inlyout = new QHBoxLayout;
    inlyout->addWidget(WDFunc::NewLBLT(this, "Отсчёт"));
    inlyout->addWidget(WDFunc::NewLBLT(this, "", "prb2lbl"));
    prb = new QProgressBar;
    prb->setObjectName("prb2prb");
    prb->setOrientation(Qt::Horizontal);
    prb->setMinimumWidth(500);
    prb->setMaximumHeight(10);
    inlyout->addWidget(prb);
    lyout->addLayout(inlyout);
    w->setLayout(lyout);
    return w;
}

#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::SetSlideWidget()
{
    QWidget *SlideWidget = new QWidget(this);
    SlideWidget->setWindowFlags(Qt::FramelessWindowHint);
    SlideWidget->setObjectName("slidew");
    SlideWidget->setStyleSheet("QWidget {background-color: rgba(110,234,145,255);}");
    QVBoxLayout *slyout = new QVBoxLayout;
    QCheckBox *chb = WDFunc::NewChB(this, "teenablechb", "Включить протокол");
    connect(chb,SIGNAL(toggled(bool)),this,SLOT(SetTEEnabled(bool)));
    slyout->addWidget(chb, 0, Qt::AlignLeft);
    QTextEdit *MainTE = new QTextEdit;
    MainTE->setObjectName("mainte");
    MainTE->setReadOnly(true);
    MainTE->setUndoRedoEnabled(false);
    MainTE->setWordWrapMode(QTextOption::WrapAnywhere);
    MainTE->document()->setMaximumBlockCount(C_TE_MAXSIZE);
    slyout->addWidget(MainTE, 40);
    SlideWidget->setLayout(slyout);
    SlideWidget->setMinimumWidth(250);
    SlideWidget->hide();
    SWGeometry = SlideWidget->geometry();
    SWHide = true;
}
#endif

void MainWindow::SetupMenubar()
{
    QMenuBar *menubar = new QMenuBar;
    QString tmps = "QMenuBar {background-color: "+QString(MAINWINCLRA1)+";}"\
            "QMenuBar::item {background-color: "+QString(MAINWINCLRA1)+";}";
    menubar->setStyleSheet(tmps);
    QMenu *menu = new QMenu;
    menu->setTitle("Главное");
    QAction *act = new QAction(this);
    act->setText("Выход");
    connect(act,SIGNAL(triggered()),this,SLOT(close()));
    menu->addAction(act);
    if (!Autonomous)
    {
        act = new QAction(this);
        act->setText("Соединение");
        act->setIcon(QIcon("images/play.png"));
        connect(act,SIGNAL(triggered()),this,SLOT(Stage1_5()));
        menu->addAction(act);
        act = new QAction(this);
        act->setText("Разрыв соединения");
        act->setIcon(QIcon("images/stop.png"));
        connect(act,SIGNAL(triggered()),this,SLOT(DisconnectAndClear()));
        menu->addAction(act);
    }
    menubar->addMenu(menu);
#if PROGSIZE >= PROGSIZE_LARGE
   /* menu = new QMenu;
    menu->setTitle("Секретные операции");
    act = new QAction(this);
    act->setText("Работа с Hidden Block");
    connect(act,SIGNAL(triggered()),this,SLOT(OpenBhbDialog()));
    menu->addAction(act);
    menubar->addMenu(menu);*/
#endif

    menu = new QMenu;
    /*menu->setTitle("Настройки");
    act = new QAction(this);
    act->setText("Настройки");
    act->setIcon(QIcon("images/settings.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(StartSettingsDialog()));
    menu->addAction(act);
    menubar->addMenu(menu);*/

    act = new QAction(this);
    act->setText("О программе");
    connect(act,SIGNAL(triggered()),this,SLOT(GetAbout()));
    menubar->addAction(act);

    menubar->addSeparator();

    setMenuBar(menubar);
    disconnected = 0;
    //AddActionsToMenuBar(menubar);
}

#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::PrepareTimers()
{
    QTimer *MouseTimer = new QTimer;
    connect(MouseTimer,SIGNAL(timeout()),this,SLOT(MouseMove()));
    MouseTimer->start(50);
}
#endif

void MainWindow::LoadSettings()
{
    QString HomeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/"+PROGNAME+"/";
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    StdFunc::SetHomeDir(sets->value("Homedir", HomeDir).toString());

    TEEnabled = sets->value("TEEnabled", "0").toBool();

}

void MainWindow::SaveSettings()
{
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    sets->setValue("Homedir", StdFunc::GetHomeDir());

    sets->setValue("TEEnabled", TEEnabled);
}

void MainWindow::ClearTW()
{
    S2Config.clear();
    ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
    if (MainTW == nullptr)
        return;
    while (MainTW->count())
    {
        QWidget *wdgt = MainTW->widget(0);
        MainTW->removeTab(0);
        wdgt->deleteLater();
    }
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != nullptr)
        MainTE->clear();
}

#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::ShowOrHideSlideSW()
{
    QWidget *w = this->findChild<QWidget *>("slidew");
    if (w == nullptr)
        return;
    if (w->isHidden())
        w->show();
    if (SWHide)
        w->setGeometry(SWGeometry);
    QPropertyAnimation *ani = new QPropertyAnimation(w, "geometry");
    ani->setDuration(500);
    QRect startRect(width(), 30, 0, height()-30);
    QRect endRect(width() - w->width(), 30, w->width(), height()-30);
    if (SWHide)
    {
        ani->setStartValue(startRect);
        ani->setEndValue(endRect);
    }
    else
    {
        ani->setStartValue(endRect);
        ani->setEndValue(startRect);
    }
    ani->start();
    SWHide = !SWHide;
}
#endif
#if PROGSIZE != PROGSIZE_EMUL
int MainWindow::CheckPassword()
{
    Ok = false;
    StdFunc::ClearCancel();
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
    connect(dlg,SIGNAL(Finished(QString)),this,SLOT(PasswordCheck(QString)));
    connect(this,SIGNAL(PasswordChecked()),&PasswordLoop,SLOT(quit()));
    dlg->show();
    PasswordLoop.exec();
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    if (!Ok)
    {
        EMessageBox::error(this, "Неправильно", "Пароль введён неверно");
        return Error::ER_GENERALERROR;
    }
    return Error::ER_NOERROR;
}
#endif


#if PROGSIZE != PROGSIZE_EMUL
void MainWindow::Stage1_5()
{
    disconnected = 0;
    ShowInterfaceDialog();
    ShowConnectDialog();
    StopRead = 0;

    if((insl.size() == 0) && ((MainInterface == "Ethernet") || (MainInterface == "RS485")))
    {
        DisconnectAndClear();
        return;
    }

    if((MainInterface == "Ethernet" && insl.at(1) != "ETH") ||
       (MainInterface == "RS485" && insl.at(1) != "MODBUS"))
    {
       EMessageBox::information(this, "Ошибка", "Выбранный интерфейс не соотвествует выбранной строке в файле");
       DisconnectAndClear();
       insl.clear();
       return;
    }


    if(MainInterface.size() != 0)
    {
        if(MainInterface == "USB")
        {
           insl.clear();

           if (Commands::Connect() != Error::ER_NOERROR)
           {
              EMessageBox::error(this, "Ошибка", "Не удалось установить связь");
              QApplication::restoreOverrideCursor();
              return;
           }
         }
    }



 QApplication::setOverrideCursor(Qt::WaitCursor);
    //connect(ch104,SIGNAL(floatsignalsready(Parse104::FLOAT104&, int N)),this,SLOT(CheckDialog84::UpdateData(Parse104::FLOAT104&, int N)));
    //connect(this,SIGNAL(stopit()),ch104,SLOT(Stop()));
#if PROGSIZE != PROGSIZE_EMUL
//#ifdef ETHENABLE
 //   ch104 = new iec104;
    //CheckDialog84* handler = new CheckDialog84;
  //  connect(ch104,SIGNAL(floatsignalsready(Parse104::Signals104*)),CheckB,SLOT(UpdateData(Parse104::Signals104*)));
  //  connect(this,SIGNAL(stopit()),ch104,SLOT(Stop()));
//#endif
#endif
    S2ConfigForTune.clear();
    S2Config.clear();
    SaveSettings();
    QApplication::restoreOverrideCursor();
    Stage2();
}

void MainWindow::Stage2()
{
    if(MainInterface.size() != 0)
    {
        //if(interface == "Ethernet и RS485")

        if(MainInterface == "USB")
        {
            int res = ModuleBSI::SetupBSI();
            if (res == Error::ER_CANAL)
            {
                if (EMessageBox::question(this, "Ошибка", \
                                          "Блок Bsi не может быть прочитан, ошибка " + QString::number(res) + ", повторить?") == 1) // Yes
                {
                    cn->Disconnect();
                    emit Retry();
                }
                return;
            }
#if PROGSIZE >= PROGSIZE_LARGE
            else if (res == Error::ER_RESEMPTY)
            {
                if (OpenBhbDialog() != Error::ER_NOERROR)
                {
                    EMessageBox::error(this, "Ошибка", "Ошибка при работе с Hidden block");
                    return;
                }
            }
            else if (res == Error::ER_NOERROR)
            {
              EMessageBox::information(this, "Успешно", "Связь с "+ModuleBSI::ModuleTypeString+" установлена");
            }
#endif
        }
    }
    //else if(interface == "Ethernet и RS485" && ch104 != nullptr)
    //emit ch104->stopall();



    Stage3();
}

void MainWindow::UpdateHthWidget(ModuleBSI::Bsi* bsi)
{
    //ModuleBSI::Bsi bsi = ModuleBSI::GetBsi();
    ModuleBSI::ModuleBsi = *bsi;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QPixmap *pm[2] = {pmred, pmgrn};

    for (int i = 0; i < MAXERRORFLAGNUM; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>(QString::number(i));  //hth+ (i+1)
        if (lbl == nullptr)
            return;
        quint32 tmpui = (0x00000001 << i) & bsi->Hth;

        WDFunc::SetLBLImage(this, (QString::number(i)), pm[tmpui]);
        /*if (tmpui)
            lbl->setStyleSheet("QLabel {background-color: rgba(255,10,10,255); color: rgba(255,255,255,255);}");
        else
            lbl->setStyleSheet("QLabel {background-color: rgba(255,50,50,0); color: rgba(220,220,220,255);}");*/
    }
}
#endif

void MainWindow::SetMainDefConf()
{
    if (MainConfDialog != nullptr)
        MainConfDialog->SetDefConf();
}

void MainWindow::SetBDefConf()
{
    if (ConfB != nullptr)
        ConfB->SetDefConf();
}

void MainWindow::SetMDefConf()
{
    if (ConfM != nullptr)
        ConfM->SetDefConf();
}

void MainWindow::Fill()
{
        if (MainConfDialog != nullptr)
        MainConfDialog->Fill();
    if (ConfB != nullptr)
        ConfB->Fill();
    if (ConfM != nullptr)
        ConfM->Fill();
}

#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::UpdateMainTE(QByteArray ba)
{
    if (!TEEnabled)
        return;
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != nullptr)
        MainTE->append(ba.toHex());
}

void MainWindow::SetTEEnabled(bool enabled)
{
    TEEnabled = enabled;
}
#endif

void MainWindow::PasswordCheck(QString psw)
{
    if (psw == "se/7520a")
        Ok = true;
    else
        Ok = false;
    emit PasswordChecked();
}

#if PROGSIZE >= PROGSIZE_LARGE
int MainWindow::OpenBhbDialog()
{
    if (!Commands::isConnected())
    {
        QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модулем" : "прибором");
        EMessageBox::information(this, "Подтверждение", "Для работы данной функции необходимо сначала установить связь с "+tmps);
        return Error::ER_GENERALERROR;
    }
    if (CheckPassword() == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;

    HiddenDialog *dlg = new HiddenDialog();
    dlg->Fill(); // заполняем диалог из недавно присвоенных значений
    dlg->exec();
    if (!dlg->ResultOk)
        return Error::ER_GENERALERROR;
    Disconnect();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (Commands::Connect() != Error::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Не удалось установить связь");
        QApplication::restoreOverrideCursor();
        return Error::ER_GENERALERROR;
    }
    QApplication::restoreOverrideCursor();
    int res;
    if ((res = ModuleBSI::SetupBSI()) == Error::ER_CANAL)
    {
        EMessageBox::error(this, "Ошибка", "Блок Bsi не может быть прочитан, ошибка " + QString::number(res));
        Commands::Disconnect();
        return Error::ER_GENERALERROR;
    }
    //emit BsiRefresh();
    return Error::ER_NOERROR;
}
#endif

#if PROGSIZE >= PROGSIZE_LARGE || PROGSIZE == PROGSIZE_EMUL
void MainWindow::StartEmul()
{
    bool ok;
    quint16 MType = sender()->objectName().toUShort(&ok, 16);
    if (!ok)
    {
        ERMSG("Wrong object name in StartEmul()");
        return;
    }
    ModuleBSI::Bsi bsi;
    bsi.MTypeB = (MType & 0xFF00) >> 8;
    bsi.MTypeM = MType & 0x00FF;
#if PROGSIZE != PROGSIZE_EMUL
   // if (cn->Connected)                             !!!!
   //     DisconnectAndClear();
#endif
    bsi.SerialNum = 0x12345678;
    bsi.Hth = 0x00;
    ModuleBSI::SetupEmulatedBsi(bsi);
    StdFunc::SetEmulated(true);
    Stage3();
}
#endif

void MainWindow::StartSettingsDialog()
{
    SettingsDialog *dlg = new SettingsDialog;
    dlg->exec();
    SaveSettings();
}

void MainWindow::ShowErrorDialog()
{
    ErrorDialog *dlg = new ErrorDialog;
    dlg->exec();
}
#if PROGSIZE != PROGSIZE_EMUL
void MainWindow::SetProgressBar1Size(int size)
{
    fileSize = size;
    SetProgressBarSize("1", size);
}

void MainWindow::SetProgressBar1(int cursize)
{
    curfileSize = cursize;
    ReceiveTimer->stop();
    ReceiveTimer->setInterval(3000);
    SetProgressBar("1", cursize);
    ReceiveTimer->start();
}

void MainWindow::FileTimeOut()
{
    QString prbname = "prb1prb";
    QString lblname = "prb1lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb == nullptr)
    {
        DBGMSG;
        return;
    }
    WDFunc::SetLBLText(this, lblname,StdFunc::PrbMessage() + QString::number(0), false);
    //prb->setMinimum(0);
    //prb->setMaximum(0);

   ReceiveTimer->stop();
   if(fileSize != curfileSize)
   {
     EMessageBox::information(this, "Ошибка", "Ошибка");
   }
}

void MainWindow::SetProgressBar2Size(int size)
{
    SetProgressBarSize("2", size);
}

void MainWindow::SetProgressBar2(int cursize)
{
    SetProgressBar("2", cursize);
}

void MainWindow::ShowInterfaceDialog()
{
    QByteArray ba;
//    int res, i;
    QDialog *dlg = new QDialog(this);
    QString Str;
    //QStringList device = QStringList() << "KDV" << "2" << "1" << "2";
    QStringList inter;
    inter.append("USB");
    inter.append("Ethernet");
    inter.append("RS485");
    QStringListModel *tmpmodel = new QStringListModel;
    tmpmodel->deleteLater();
    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addWidget(WDFunc::NewLBL(this, "Выберите интерфейс связи"));

    //tmpmodel = new QStringListModel;
    tmpmodel->setStringList(inter);
    QComboBox *portscb = new QComboBox;
    connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(ParseInter(QString)));
    portscb->setModel(tmpmodel);
    lyout->addWidget(portscb);
    QHBoxLayout *hlyout = new QHBoxLayout;
/*    QPushButton *pb = new QPushButton("Далее");
    connect(pb, SIGNAL(clicked(bool)),dlg,SLOT(close()));
    hlyout->addWidget(pb); */
    hlyout->addWidget(WDFunc::NewPB(this, "", "Далее", dlg, SLOT(close())));
/*    pb = new QPushButton("Отмена");
    //connect(pb, SIGNAL(clicked(bool)),cn,SLOT(SetCancelled()));         !!!
    connect(pb, SIGNAL(clicked(bool)),dlg, SLOT(close()));
    hlyout->addWidget(pb); */
    hlyout->addWidget(WDFunc::NewPB(this, "", "Отмена", dlg, SLOT(close())));
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    dlg->exec();
}

void MainWindow::ShowConnectDialog()
{
    QByteArray ba;
    int res;
    QDialog *dlg = new QDialog(this);
    QString Str;
    //QStringList device = QStringList() << "KDV" << "2" << "1" << "2";
    //QStringList inter = QStringList() << "ETH" << "MODBUS";

    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    QVBoxLayout *lyout = new QVBoxLayout;   
    QStringListModel *tmpmodel = new QStringListModel;

    if(MainInterface.size() != 0)
    {
         if(MainInterface == "USB")
         {
             cn = new EUsbHid;
             connect(cn,SIGNAL(Retry()),this,SLOT(ShowConnectDialog()));
             connect(cn,SIGNAL(SetDataSize(int)),this,SLOT(SetProgressBar1Size(int)));
             connect(cn,SIGNAL(SetDataCount(int)),this,SLOT(SetProgressBar1(int)));
             connect(cn,SIGNAL(readbytessignal(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
             connect(cn,SIGNAL(writebytessignal(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
             connect(cn, SIGNAL(ShowError(QString)), this, SLOT(ShowErrorMessageBox(QString)));
             connect(this,SIGNAL(Retry()),this,SLOT(Stage1_5()));

            USBsl = cn->DevicesFound();
            if (USBsl.size() == 0)
            {
                lyout->addWidget(WDFunc::NewLBL(this, "Ошибка, устройства не найдены"));
                Error::ShowErMsg(CN_NOPORTSERROR);
            }
            tmpmodel->deleteLater();
            tmpmodel->setStringList(USBsl);
            QComboBox *portscb = new QComboBox;
            connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetPortSlot(QString)));
            portscb->setModel(tmpmodel);
            lyout->addWidget(portscb);
         }
         else if(MainInterface == "Ethernet" || MainInterface == "RS485")
         {

            /*tmpmodel->setStringList(device);
            QComboBox *portscb = new QComboBox;
            connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SDevice(QString)));
            portscb->setModel(tmpmodel);
            lyout->addWidget(portscb);
            QHBoxLayout *hlyout = new QHBoxLayout;
            QPushButton *pb = new QPushButton("Далее");
            connect(pb, SIGNAL(clicked(bool)),dlg,SLOT(close()));
            hlyout->addWidget(pb);
            pb = new QPushButton("Отмена");
            //connect(pb, SIGNAL(clicked(bool)),cn,SLOT(SetCancelled()));         !!!
            connect(pb, SIGNAL(clicked(bool)),dlg, SLOT(close()));
            hlyout->addWidget(pb);
            lyout->addLayout(hlyout);
            dlg->setLayout(lyout);
            dlg->exec();

            dlg = new QDialog(this);

            tmpmodel = new QStringListModel;
            tmpmodel->setStringList(inter);
            portscb = new QComboBox;
            lyout = new QVBoxLayout;
            connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SaveInterface(QString)));
            portscb->setModel(tmpmodel);
            lyout->addWidget(portscb);
            hlyout = new QHBoxLayout;
            pb = new QPushButton("Далее");
            connect(pb, SIGNAL(clicked(bool)),dlg,SLOT(close()));
            hlyout->addWidget(pb);
            pb = new QPushButton("Отмена");
            //connect(pb, SIGNAL(clicked(bool)),cn,SLOT(SetCancelled()));         !!!
            connect(pb, SIGNAL(clicked(bool)),dlg, SLOT(close()));
            hlyout->addWidget(pb);
            lyout->addLayout(hlyout);
            dlg->setLayout(lyout);
            dlg->exec();*/


            if(!HaveAlreadyRed)
            {
                sl.clear();
                res= Files::LoadFromFile(Files::ChooseFileForOpen(this, "IP files (*.txt)"), ba);
                if (res != Files::ER_NOERROR)
                {
                   WARNMSG("Ошибка при загрузке файла");
                   return;
                }


                Str = ba;
                sl.append(Str.split("\r\n"));

                /*for(i=0; i<sl.size(); i++)
                {
                  insl.clear();
                  insl.append(sl.at(i).split(" "));

                   if((insl.at(0) == SaveDevice) && (insl.at(1) == interface))
                   {
                     slfinal.append(sl.at(i));
                   }
                }*/

                HaveAlreadyRed = 1;

            }


            if (sl.size() == 0)
            {
                lyout->addWidget(WDFunc::NewLBL(this, "Ошибка, устройства не найдены"));
                Error::ShowErMsg(CN_NOPORTSERROR);
            }

            //dlg = new QDialog(this);

            //tmpmodel = new QStringListModel;

             tmpmodel->deleteLater();
             tmpmodel->setStringList(sl);

             QComboBox *portscb = new QComboBox;
             connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(ParseString(QString)));
             portscb->setModel(tmpmodel);
             lyout->addWidget(portscb);
         }
    }
        QHBoxLayout *hlyout = new QHBoxLayout;
        QPushButton *pb = new QPushButton("Далее");
        connect(pb, SIGNAL(clicked(bool)),dlg,SLOT(close()));
        hlyout->addWidget(pb);
        pb = new QPushButton("Отмена");
        if(MainInterface == "USB")
        connect(pb, SIGNAL(clicked(bool)),cn,SLOT(SetCancelled()));

        connect(pb, SIGNAL(clicked(bool)),dlg, SLOT(close()));
        hlyout->addWidget(pb);
        lyout->addLayout(hlyout);
        dlg->setLayout(lyout);
        dlg->exec();

}

void MainWindow::SetProgressBarSize(QString prbnum, int size)
{
    QString prbname = "prb"+prbnum+"prb";
    QString lblname = "prb"+prbnum+"lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb == nullptr)
    {
        DBGMSG;
        return;
    }
    WDFunc::SetLBLText(this, lblname,StdFunc::PrbMessage() + QString::number(size), false);
    prb->setMinimum(0);
    prb->setMaximum(size);
}

void MainWindow::SetProgressBar(QString prbnum, int cursize)
{
    QString prbname = "prb"+prbnum+"prb";
    QString lblname = "prb"+prbnum+"lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb != nullptr)
    {
        prb->setValue(cursize);
        WDFunc::SetLBLText(this, lblname, StdFunc::PrbMessage() + QString::number(cursize) + " из " + QString::number(prb->maximum()));
    }
}
#endif

void MainWindow::GetAbout()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *l2yout = new QVBoxLayout;
    QString tmps = QString(PROGCAPTION);

    l2yout->addWidget(WDFunc::NewLBL(this, tmps));
    l2yout->addWidget(WDFunc::NewLBL(this, "ООО \"АВМ-Энерго\""));
    l2yout->addWidget(WDFunc::NewLBL(this, "2015-2019 гг."));
    l2yout->addWidget(WDFunc::NewLBL(this, "info@avmenergo.ru"));
    l2yout->addStretch(10);
    hlyout->addWidget(WDFunc::NewLBL(this, "", "", "", new QPixmap("images/evel.png")), 1, Qt::AlignVCenter);
    hlyout->addLayout(l2yout,100);
    lyout->addLayout(hlyout,1);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb,0);
    dlg->setLayout(lyout);
    dlg->exec();
}
#if PROGSIZE != PROGSIZE_EMUL
void MainWindow::Disconnect()
{
    emit stoptime();
    if(MainInterface.size() != 0 && (!StdFunc::IsInEmulateMode()))
    {
        if(MainInterface == "USB")
        {
         if(BdaTimer != nullptr)
         BdaTimer->stop();
         TimeFunc::Wait(100);
         cn->Disconnect();
        }
        else
        {
         emit stopit();
         emit stopModBus();
        }

    }
}

void MainWindow::GetDeviceFromTable(QModelIndex idx)
{
    Q_UNUSED(idx)
    ETableView *tv = this->findChild<ETableView *>("devicetv");
    if (tv == nullptr)
    {
        DBGMSG;
        return;
    }
    QString tmps = tv->model()->index(tv->currentIndex().row(), 0, QModelIndex()).data(Qt::DisplayRole).toString();
    DevInfo.vendor_id = tmps.toUShort(nullptr, 16);
//    quint16 vid = tmps.toInt(nullptr, 16);
    tmps = tv->model()->index(tv->currentIndex().row(), 1, QModelIndex()).data(Qt::DisplayRole).toString();
    DevInfo.product_id = tmps.toUShort(nullptr, 16);
//    quint16 pid = tmps.toInt(nullptr, 16);
    tmps = tv->model()->index(tv->currentIndex().row(), 3, QModelIndex()).data(Qt::DisplayRole).toString();
    tmps.toWCharArray(DevInfo.serial);
}
#endif
void MainWindow::DisconnectAndClear()
{
    if(!disconnected)
    {
        StopRead = 1;
        disconnected = 1;
    #if PROGSIZE != PROGSIZE_EMUL
        Disconnect();
    #endif

        CheckB = CheckM = nullptr;
        CheckModBus = nullptr;
        emit ClearBsi();
        ClearTW();
        ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
        if (MainTW == nullptr)
            return;
        MainTW->hide();
        StdFunc::SetEmulated(false);

        //if(thr != nullptr)
        //emit FinishAll();

        //thr = nullptr;
        //Time = nullptr;

        if(MainInterface.size() != 0)
        {
           if(MainInterface == "USB")
           {
             EMessageBox::information(this, "Разрыв связи", "Связь с "+ModuleBSI::ModuleTypeString+" разорвана");
           }
           else
           {
             EMessageBox::information(this, "Разрыв связи", "Связь с "+FullName+" разорвана");
           }
        }
        //disconnected = 0;
    }


}

#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::MouseMove()
{
    QPoint curPos = mapFromGlobal(QCursor::pos());
    QWidget *sww = this->findChild<QWidget *>("slidew");
    if (sww == nullptr)
        return;
    if ((abs(curPos.x() - width()) < 10) && (curPos.y() > 0) && (curPos.y() < height()))
    {
        if (SWHide)
            ShowOrHideSlideSW();
    }
    else if ((abs(curPos.x() - width()) > sww->width()) && (curPos.y() > 0) && (curPos.y() < height()))
    {
        if (!SWHide)
            ShowOrHideSlideSW();
    }
}
#endif

void MainWindow::ShowErrorMessageBox(QString message)
{
    EMessageBox::error(this, "Ошибка", message);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    if (!SWHide)
    {
        QWidget *sww = this->findChild<QWidget *>("slidew");
        if (sww == nullptr)
            return;
        sww->setGeometry(QRect(width()-sww->width(), 0, sww->width(), height()));
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
        emit Finished();
    if (e->key() == Qt::Key_Escape)
        StdFunc::Cancel();
    QMainWindow::keyPressEvent(e);
}

void MainWindow::ParseString(QString Str)
{ 
    insl.clear();
    //if(Str != nullptr)
    insl.append(Str.split(" "));

    if (insl.size() < 2)
    {
        EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
        return;
    }
    if(insl.at(1) == "ETH")
    {
       Disconnect();
       FullName = insl.at(0);
       AdrBaseStation = insl.at(2).toUShort();
       IPtemp = Str.split(" ").last();
    }
    else if(insl.at(1) == "MODBUS")
    {
        if(insl.size() == 7)
        {
            FullName = insl.at(0);
            Settings.baud =  insl.at(2);
            Settings.parity =  insl.at(3);
            Settings.stop =  insl.at(4);
            Settings.adr =  insl.at(5);
            Settings.port =  insl.at(6);
        }
        else
        {
            EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
        }
    }
}

void MainWindow::ParseInter(QString Str)
{
    MainInterface = Str;
}

/*void MainWindow::SaveModBusString(QString ModBus)
{

}


void MainWindow::SaveInterface(QString Interface)
{
    interface = Interface;
}

void MainWindow::SDevice(QString Device)
{
    SaveDevice = Device;
}*/
void MainWindow::SetPortSlot(QString port)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->TranslateDeviceAndSave(port);
#endif
}

void MainWindow::SetDefConf()
{
    SetMainDefConf();
    SetBDefConf();
    SetMDefConf();
    Fill();
    //EMessageBox::information(this, "Информация", "В модуле нет конфигурации. \nНеобходимо записать конфигурацию по умолчанию.");
    //if(ConfB != nullptr)
    //ConfB->WriteConf();
    //if(ConfM != nullptr)
    //ConfM->WriteConf();
    EMessageBox::information(this, "Успешно", "Конфигурация по умолчанию");
}


void MainWindow::ConnectMessage()
{
   EMessageBox::information(this, "Успешно", "Связь с "+FullName+" установлена");
}

void MainWindow::DisconnectMessage()
{
    EMessageBox::information(this, "Провал", "Нет подключения");
}

void MainWindow::FinishHim()
{
    //Time->closeThr = true;
    //thr->exit();
    //emit Time->finished();

    //thr->wait(100);
    //thr->deleteLater();
    ConfM->timeIndex = -1;
    ConfM->confIndex = -1;
    TimeFunc::Wait(1000);
    //ConfM->stopRead(ConfM->timeIndex);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    TheEnd = 1;
    DisconnectAndClear();
    //while(!TimeThrFinished || !ModBusThrFinished)
    //TimeFunc::Wait(100);
    event->accept();
}

void MainWindow::CheckTimeFinish()
{
   TimeThrFinished = true;
}

void MainWindow::CheckModBusFinish()
{
   TimeThrFinished = true;
}

void MainWindow::Stop_BdaTimer(int index)
{
    if(CheckB != nullptr)
    {
        if(index != CheckB->checkIndex)
        {
            BdaTimer->stop();
            //thr->msleep(100);
        }
    }

}

void MainWindow::Start_BdaTimer(int index)
{
    if(CheckB != nullptr)
    {
        if(index == CheckB->checkIndex)
        {
            BdaTimer->start();
            //thr->msleep(100);
        }
    }

}
