#include <QTime>
#include <QTabWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QMessageBox>
#include <QPushButton>
#include <QInputDialog>
#include <QFileDialog>
#include <QThread>
#include <QCoreApplication>
#include "e_tunedialog.h"
#include "../publicclass.h"
#include "../canal.h"
#include "e_config.h"

e_tunedialog::e_tunedialog(QWidget *parent) :
    QDialog(parent)
{
    tmr = new QTimer;
    tmr->setInterval(ANMEASINT);
    connect(tmr,SIGNAL(timeout()),this,SLOT(ReadAnalogMeasurements()));
    MipErrNeeded = true;
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

void e_tunedialog::SetupUI()
{
    int i;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: blue; font: bold 10px;}";
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
    QWidget *cp4 = new QWidget;
    QWidget *cp5 = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl;
    QGridLayout *glyout = new QGridLayout;

    QTabWidget *TuneTW = new QTabWidget;
    TuneTW->addTab(cp1,"Настройка");
    TuneTW->addTab(cp2,"Коэффициенты");
    TuneTW->addTab(cp3,"Данные МИП");
    TuneTW->addTab(cp4,"Данные 104");
    TuneTW->addTab(cp5,"Данные модуля");

    // CP1 - НАСТРОЙКА МОДУЛЯ

    lyout = new QVBoxLayout;
    QPushButton *pb = new QPushButton("Начать настройку");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartTune()));
    lyout->addWidget(pb);
    QStringList lbls;
    lbls << "7.2.3. Проверка связки РЕТОМ и МИП..." << "7.3.1. Получение настроечных параметров..." \
            "7.3.1.1. Установка настроечных параметров по умолчанию..." << "7.3.2. Получение текущих аналоговых данных..." \
            "7.3.3. Расчёт коррекции смещений сигналов по фазе..." << "7.3.4. Расчёт коррекции по частоте..." \
            "7.3.5. Пересборка схемы подключения..." << "7.3.6.1. Получение текущих аналоговых данных..." \
            "7.3.6.2. Расчёт коррекции взаимного влияния каналов..." << "7.3.7.1.1. Получение текущих аналоговых данных..." \
            "7.3.7.1.2. Расчёт калибровочных коэффициентов по напряжениям..." << "7.3.7.2. Сохранение конфигурации и перезапуск..." \
            "7.3.7.3. Получение текущих аналоговых данных..." << "7.3.7.5. Расчёт калибровочных коэффициентов по токам, напряжениям и углам..." \
            "7.3.7.6. Сохранение конфигурации и перезапуск..." << "7.3.7.8. Получение текущих аналоговых данных..." \
            "7.3.7.10. Расчёт калибровочных коэффициентов по токам, напряжениям и углам..." << "7.3.8.1. Запись настроечных коэффициентов..." \
            "7.3.8.2. Проверка аналоговых данных..." << "7.3.9. Восстановление сохранённой конфигурации...";
    for (int i = 0; i < lbls.size(); i++)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        lbl=new QLabel(lbls.at(i));
        lbl->setVisible(false);
        lbl->setObjectName("tunemsg"+QString::number(i));
        hlyout->addWidget(lbl);
        lbl=new QLabel("");
        lbl->setVisible(false);
        lbl->setObjectName("tunemsgres"+QString::number(i));
        hlyout->addWidget(lbl);
        hlyout->addStretch(1);
        lyout->addLayout(hlyout);
    }
    lyout->addStretch(1);
    cp1->setLayout(lyout);

    // CP2 - КОЭФФИЦИЕНТЫ МОДУЛЯ

    QGroupBox *gb = new QGroupBox("Настроечные коэффициенты");
    for (i = 0; i < 6; i++)
    {
        lbl = new QLabel("KmU["+QString::number(i)+"]");
        glyout->addWidget(lbl,0,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("tune"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        glyout->addWidget(lbl,1,i,1,1);
        lbl = new QLabel("KmI_5["+QString::number(i)+"]");
        glyout->addWidget(lbl,2,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("tune"+QString::number(i+6));
        lbl->setStyleSheet(ValuesFormat);
        glyout->addWidget(lbl,3,i,1,1);
        lbl = new QLabel("KmI_1["+QString::number(i)+"]");
        glyout->addWidget(lbl,4,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("tune"+QString::number(i+12));
        lbl->setStyleSheet(ValuesFormat);
        glyout->addWidget(lbl,5,i,1,1);
        lbl = new QLabel("DPsi["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("tune"+QString::number(i+18));
        lbl->setStyleSheet(ValuesFormat);
        glyout->addWidget(lbl,7,i,1,1);
    }
    lbl=new QLabel("K_freq:");
    lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,8,0,1,1);
    lbl = new QLabel("");
    lbl->setObjectName("tune24");
    lbl->setStyleSheet(ValuesFormat);
    glyout->addWidget(lbl,8,1,1,2);
    lbl=new QLabel("Kinter:");
    lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,8,3,1,1);
    lbl = new QLabel("");
    lbl->setObjectName("tune25");
    lbl->setStyleSheet(ValuesFormat);
    glyout->addWidget(lbl,8,4,1,2);

    pb = new QPushButton("Прочитать настроечные коэффициенты из модуля");
    connect(pb,SIGNAL(clicked()),this,SLOT(ReadTuneCoefs()));
    glyout->addWidget(pb, 9, 0, 1, 6);
    pb = new QPushButton("Записать настроечные коэффициенты в модуль");
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteTuneCoefs()));
    glyout->addWidget(pb, 10, 0, 1, 6);
    pb = new QPushButton("Прочитать настроечные коэффициенты из файла");
    pb->setEnabled(false);
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadFromFile()));
    glyout->addWidget(pb, 11, 0, 1, 6);
    pb = new QPushButton("Записать настроечные коэффициенты в файл");
    pb->setEnabled(false);
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveToFile()));
    glyout->addWidget(pb, 12, 0, 1, 6);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    lyout->addStretch(1);
    cp2->setLayout(lyout);

    // CP3 - ПОКАЗАНИЯ МИП-02

    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hglyout = new QHBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    gb = new QGroupBox("Измеряемые параметры");
    QGroupBox *gb2 = new QGroupBox("Частота");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+1));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    gb2 = new QGroupBox("Фазное напряжение");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+4));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    gb2 = new QGroupBox("Фазный ток");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+7));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    gb2 = new QGroupBox("Угол нагрузки");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+11));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    gb2 = new QGroupBox("Фазовый угол напряжения");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+14));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    gb2 = new QGroupBox("Прочие");
    lbl = new QLabel("Ток N");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip10");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    lbl = new QLabel("Темп.");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip17");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    gb->setLayout(vlyout);
    hglyout->addWidget(gb);
    gb = new QGroupBox("Вычисляемые параметры");
    gb2 = new QGroupBox("Активная мощность");
    hlyout = new QHBoxLayout;
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+22));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    QVBoxLayout *gblyout = new QVBoxLayout;
    gblyout->addWidget(gb2);
    gb2 = new QGroupBox("Реактивная мощность");
    hlyout = new QHBoxLayout;
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+26));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    gblyout->addWidget(gb2);
    gb2 = new QGroupBox("Полная мощность");
    hlyout = new QHBoxLayout;
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+30));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    gblyout->addWidget(gb2);
    gb2 = new QGroupBox("Линейное напряжение");
    hlyout = new QHBoxLayout;
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+19));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    gblyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    vlyout=new QVBoxLayout;
    gb2 = new QGroupBox("Мощность");
    lbl = new QLabel("Акт:");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip25");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    lbl = new QLabel("Реакт:");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip29");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    vlyout->addLayout(hlyout);
    hlyout=new QHBoxLayout;
    lbl = new QLabel("Полная:");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip33");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    vlyout->addLayout(hlyout);
    vlyout->addStretch(1);
    gb2->setLayout(vlyout);
    gblyout->addWidget(gb2,1);
    gb->setLayout(gblyout);
    hglyout->addWidget(gb);
    vlyout = new QVBoxLayout;
    vlyout->addLayout(hglyout);
    pb = new QPushButton("Запустить связь с МИП");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartMip()));
    vlyout->addWidget(pb);
    pb = new QPushButton("Остановить связь с МИП");
    connect(pb,SIGNAL(clicked()),this,SLOT(StopMip()));
    vlyout->addWidget(pb);
    cp3->setLayout(vlyout);

    // CP4 - ДАННЫЕ 104

    lyout=new QVBoxLayout;

    // CP5 - ДАННЫЕ МОДУЛЯ

    lyout = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    lbl = new QLabel("Температура:");
    hlyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("value0");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,1);
    lbl = new QLabel("Частота:");
    hlyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("value1");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,1);
    lyout->addLayout(hlyout);
    glyout = new QGridLayout;
    for (i = 0; i < 6; i++)
    {
        lbl = new QLabel("IUNF["+QString::number(i)+"]");
        glyout->addWidget(lbl,0,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+2));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Истинные действующие значения сигналов");
        glyout->addWidget(lbl,1,i,1,1);
        lbl = new QLabel("IUF["+QString::number(i)+"]");
        glyout->addWidget(lbl,2,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+8));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Действующие значения сигналов по 1-й гармонике");
        glyout->addWidget(lbl,3,i,1,1);
        lbl = new QLabel("PHF["+QString::number(i)+"]");
        glyout->addWidget(lbl,4,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+14));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Угол сдвига между сигналами по первой гармонике");
        glyout->addWidget(lbl,5,i,1,1);
    }
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("PNF["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+20));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Истинная активная мощность");
        glyout->addWidget(lbl,7,i,1,1);
        lbl = new QLabel("SNF["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i+3,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+23));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Кажущаяся полная мощность");
        glyout->addWidget(lbl,7,i+3,1,1);
        lbl = new QLabel("QNF["+QString::number(i)+"]");
        glyout->addWidget(lbl,8,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+26));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Реактивная мощность");
        glyout->addWidget(lbl,9,i,1,1);
        lbl = new QLabel("Cos["+QString::number(i)+"]");
        glyout->addWidget(lbl,8,i+3,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+29));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Cos phi");
        glyout->addWidget(lbl,9,i+3,1,1);
        lbl = new QLabel("PF["+QString::number(i)+"]");
        glyout->addWidget(lbl,10,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+32));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Активная мощность по 1-й гармонике");
        glyout->addWidget(lbl,11,i,1,1);
        lbl = new QLabel("QF["+QString::number(i)+"]");
        glyout->addWidget(lbl,10,i+3,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+35));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Реактивная мощность по 1-й гармонике");
        glyout->addWidget(lbl,11,i+3,1,1);
        lbl = new QLabel("SF["+QString::number(i)+"]");
        glyout->addWidget(lbl,12,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+38));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Полная мощность по 1-й гармонике");
        glyout->addWidget(lbl,13,i,1,1);
        lbl = new QLabel("CosPhi["+QString::number(i)+"]");
        glyout->addWidget(lbl,12,i+3,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+41));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Cos phi по 1-й гармонике");
        glyout->addWidget(lbl,13,i+3,1,1);
    }
    lyout->addLayout(glyout);
    pb = new QPushButton("Запустить чтение аналоговых сигналов");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartAnalogMeasurements()));
    lyout->addWidget(pb);
    pb = new QPushButton("Остановить чтение аналоговых сигналов");
    connect(pb,SIGNAL(clicked()),this,SLOT(StopAnalogMeasurements()));
    lyout->addWidget(pb);

    lyout->addStretch(1);
    cp5->setLayout(lyout);

    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);
}

void e_tunedialog::StartTune()
{
    bool res;
    // показываем диалог с выбором метода контроля
    ShowControlChooseDialog();
    // показываем диалог со схемой подключения
    Show1PhaseScheme();
    // показываем диалог с требованием установки необходимых сигналов на РЕТОМ
    Show1RetomDialog(1);

    // 7.2.3. проверка связи РЕТОМ и МИП
    if (TuneControlType == TUNEMIP)
        res = Start7_2_3();
    if (!res)
        return;

    // 7.3.1. получение настроечных коэффициентов
    res = Start7_3_1();
    if (!res)
        return;

    // 7.3.2. получение аналоговых сигналов
    res = Start7_3_2(3);
    if (!res)
        return;
    // сохраняем значения по п. 7.3.2 для выполнения п. 7.3.6
    for (int i=0; i<6; i++)
        IUefNat_filt_old[i] = Bda_block.IUefNat_filt[i];

    // 7.3.3. расчёт коррекции по фазе и по частоте
    res = Start7_3_3();
    if (!res)
        return;

    // 7.3.4. расчёт коррекции по частоте
    res = Start7_3_4();
    if (!res)
        return;

    // 7.3.5. пересборка схемы
    Show3PhaseScheme();

    // 7.3.6.1. получение аналоговых данных
    res = Start7_3_2(7);
    if (!res)
        return;

    // 7.3.6.2. расчёт коррекции влияния каналов
    Start7_3_6_2();

    if (pc.MType1 == MTE_0T2N)
    {
        // 7.3.7.1. расчёт калибровочных коэффициентов по напряжениям
        res = Start7_3_7_1();
        if (!res)
            return;
    }
    else
    {
        // 7.3.7.2. расчёт калибровочных коэффициентов по напряжениям
        res = Start7_3_7_2();
        if (!res)
            return;
    }

}

void e_tunedialog::SetTuneManual()
{
    TuneControlType=TUNEMAN;
}

void e_tunedialog::SetTuneMip()
{
    TuneControlType=TUNEMIP;
}

void e_tunedialog::SetTuneRetom()
{
    TuneControlType=TUNERET;
}

void e_tunedialog::ShowControlChooseDialog()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl = new QLabel("Выберите метод подтверждения измеряемых данных:");
    lyout->addWidget(lbl);
    QPushButton *pb = new QPushButton("Автоматически по показаниям МИП-02");
    TuneControlType = TUNERET; // по-умолчанию тип контроля - по РЕТОМу
    connect(pb,SIGNAL(clicked()),this,SLOT(SetTuneMip()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Вручную");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetTuneManual()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Автоматически по прибору РЕТОМ");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetTuneRetom()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
}

void e_tunedialog::Show1PhaseScheme()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QPixmap pmp;
    switch(pc.MType1) // выводим окно с предупреждением о включении РЕТОМ-а по схеме в зависимости от исполнения
    {
    case MTE_2T0N:
    {
        pmp.load(":/mte_2t0n.png");
        break;
    }
    case MTE_1T1N:
    {
        pmp.load(":/mte_1t1n.png");
        break;
    }
    case MTE_0T2N:
    {
        pmp.load(":/mte_0t2n.png");
        break;
    }
    default:
        return;
        break;
    }
    QLabel *lbl = new QLabel("1.Соберите схему подключения по нижеследующей картинке:");
    QLabel *lblpmp = new QLabel;
    lblpmp->setPixmap(pmp);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(lbl);
    lyout->addWidget(lblpmp);
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
}

void e_tunedialog::Show1RetomDialog(int A)
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl = new QLabel("2.Вызовите программу управления РЕТОМ и войдите в режим \"Ручное управление выходами\";\n"\
                              "3.При выключенных выходах РЕТОМ задайте частоту 50,0 Гц, трёхфазные напряжения на уровне 60,0 В с фазой 0 градусов;");
    lyout->addWidget(lbl);
    if (pc.MType1!=MTE_0T2N)
    {
        lbl=new QLabel("4.Задайте трёхфазные токи на уровне "+QString::number(A)+" А с фазой 0 градусов;");
        lyout->addWidget(lbl);
    }
    if (TuneControlType == TUNEMIP)
    {
        lbl=new QLabel("5.Включите питание прибора МИП-02;");
        lyout->addWidget(lbl);
    }
    lbl=new QLabel("6.Включите выходы РЕТОМ");
    lyout->addWidget(lbl);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
}

void e_tunedialog::Show3PhaseScheme()
{
    // высвечиваем надпись "Проверка связи РЕТОМ и МИП"
    ShowMsg(6);
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QPixmap pmp;
    switch(pc.MType1) // выводим окно с предупреждением о включении РЕТОМ-а по схеме в зависимости от исполнения
    {
    case MTE_2T0N:
    {
        pmp.load(":/mte3-e2t0n.png");
        break;
    }
    case MTE_1T1N:
    {
        pmp.load(":/mte3-e1t1n.png");
        break;
    }
    case MTE_0T2N:
    {
        pmp.load(":/mte3-e2t0n.png");
        break;
    }
    default:
        return;
        break;
    }
    QLabel *lbl = new QLabel("1.Соберите схему подключения по нижеследующей картинке:");
    QLabel *lblpmp = new QLabel;
    lblpmp->setPixmap(pmp);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(lbl);
    lyout->addWidget(lblpmp);
    lbl=new QLabel("при этом следует отключить выходы РЕТОМ, перебрать схему подключения \n" \
                   "входов модуля в соответствии с картинкой, и вновь включить выходы РЕТОМ \n" \
                   "(не меняя уставок напряжения и тока!)");
    lyout->addWidget(lbl);
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
    ShowOkMsg(6);
}

bool e_tunedialog::Start7_2_3()
{
    // высвечиваем надпись "Проверка связи РЕТОМ и МИП"
    ShowMsg(0);
    MipErrNeeded = false; // запрещаем вывод сообщений об ошибках связи с МИП
    StartMip();
    QTime tmr;
    tmr.start();
    while (tmr.elapsed() < 5000)
        qApp->processEvents();
    StopMip();
    if (CheckMip())
        ShowOkMsg(0);
    else
    {
        ShowErMsg(0);
        return false;
    }
    return true;
}

bool e_tunedialog::Start7_3_1()
{
    // высвечиваем надпись "Получение настроечных коэффициентов"
    ShowMsg(1);
    // получение настроечных коэффициентов от модуля
    cn->Send(CN_Gac, &Bac_block, sizeof(Bac_block));
    while (cn->Busy)
        qApp->processEvents();
    if (cn->result != CN_OK)
        return false;
    // обновление коэффициентов в соответствующих полях на экране
    RefreshTuneCoefs();
    // проверка коэффициентов на правильность в соотв. с п. 7.3.1 "Д2"
    if (CheckTuneCoefs())
        ShowOkMsg(1);
    else
    {
        ShowErMsg(1);
        // высвечиваем надпись "Запись настроечных коэффициентов по умолчанию"
        ShowMsg(2);
        // запись настроечных коэффициентов в модуль
        cn->Send(CN_Wac, &Bac_defblock, sizeof(Bac));
        while (cn->Busy)
            qApp->processEvents();
        if (cn->result == CN_OK)
            ShowOkMsg(2);
        else
        {
            ShowErMsg(2);
            return false;
        }
    }
    return true;
}

bool e_tunedialog::Start7_3_2(int num)
{
    // высвечиваем надпись "Получение текущих аналоговых данных"
    ShowMsg(num);
    ReadAnalogMeasurements();
    // проверка данных на правильность
    int maxval = ((num == 3) || (num == 6) || (num == 12)) ? 602 : 572; // 3~7.3.2, 6~7.3.6.1, 12~7.3.7.3, 17~7.3.7.8
    if (CheckAnalogValues(maxval-pc.MType1)) // MType: 0 = 2T0N, 1 = 1T1N, 2 = 0T2N; NTest: 600 = 0T2N, 601 = 1T1N, 602 = 2T0N
        ShowOkMsg(num);
    else
    {
        ShowErMsg(num);
        return false;
    }
    return true;
}

bool e_tunedialog::Start7_3_3()
{
    // высвечиваем надпись "7.3.3. Расчёт коррекции смещений сигналов по фазе"
    ShowMsg(4);
    double fTmp = Bda_block.phi_next_f[0];
    int k = (pc.MType1 == MTE_1T1N) ? 3 : 6;
    for (int i=1; i<k; i++)
    {
        Bac_newblock.DPsi[i] = Bac_block.DPsi[i] - fTmp;
        fTmp += Bda_block.phi_next_f[i];
    }
    ShowOkMsg(4);
    return true;
}

bool e_tunedialog::Start7_3_4()
{
    // высвечиваем надпись "7.3.4. Расчёт коррекции по частоте"
    ShowMsg(5);
    switch (TuneControlType)
    {
    case TUNEMIP:
    {
        StartMip();
        QTime tmr;
        tmr.start();
        while (tmr.elapsed() < 5000)
            qApp->processEvents();
        StopMip();
        if (CheckMip())
        {
            double fTmp = MipDat[1]; // частота ф. А
            Bac_newblock.K_freq = Bac_block.K_freq*fTmp/Bda_block.Frequency;
            ShowOkMsg(5);
        }
        else
        {
            ShowErMsg(5);
            return false;
        }
        break;
    }
    case TUNERET:
    {
        Bac_newblock.K_freq = Bac_block.K_freq*50.0/Bda_block.Frequency; // считаем, что частота 50 Гц
        ShowOkMsg(5);
        break;
    }
    case TUNEMAN:
    {
        double fTmp = QInputDialog::getDouble(this,"Ввод числа","Введите показания приборов по частоте",50.0,40.0,60.0,5);
        Bac_newblock.K_freq = Bac_block.K_freq*fTmp/Bda_block.Frequency; // считаем, что частота 50 Гц
        ShowOkMsg(5);
        break;
    }
    }
    return true;
}

void e_tunedialog::Start7_3_6_2()
{
    // высвечиваем надпись "7.3.6.2. Расчёт коррекции взаимного влияния"
    ShowMsg(8);
    double fTmp = 0.0;
    for (int i=0; i<4; i+=3)
        fTmp += (Bda_block.IUefNat_filt[i]/IUefNat_filt_old[i]);
    fTmp /= 2.0;
    Bac_newblock.Kinter = (fTmp * (1.0 + 6.0*Bac_block.Kinter) -1.0) / 6.0;
    ShowOkMsg(8);
}

bool e_tunedialog::Start7_3_7_1()
{
    // высвечиваем надпись "7.3.7.1.1"
    ShowMsg(9);
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl = new QLabel("1.На выходах РЕТОМ задайте частоту 50,0 Гц, трёхфазные напряжения на уровне 60,0 В с фазой 0 градусов");
    lyout->addWidget(lbl);
    lbl=new QLabel("2.Включите выходы РЕТОМ");
    lyout->addWidget(lbl);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();

    GetExternalData(9);

    // высвечиваем надпись "7.3.7.1.2"
    ShowMsg(10);
    for (int i=0; i<3; i++)
    {
        Bac_newblock.KmU[i] = Bac_block.KmU[i] * mipd[i] / Bda_block.IUefNat_filt[i];
        Bac_newblock.KmU[i+3] = Bac_block.KmU[i+3] * mipd[i] / Bda_block.IUefNat_filt[i+3];
    }
    ShowOkMsg(10);
    return true;
}

bool e_tunedialog::Start7_3_7_2()
{
    // 1. установить в конфигурации токи i2nom в 1 А и перезагрузить модуль
    ShowMsg(11);
    e_config *econf = new e_config;
    e_config::Bci Bci_block_work;

    cn->Send(CN_GF,NULL,0,1,econf->Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        memcpy(&Bci_block_work,&econf->Bci_block,sizeof(e_config::Bci));
    else
        return false;
    for (int i=0; i<6; i++)
        econf->Bci_block.inom2[i] = 1.0;
    cn->Send(CN_WF, &econf->Bci_block, sizeof(e_config::Bci), 2, econf->Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result != CN_OK)
        return false;
    cn->Send(CN_Cnc);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result != CN_OK)
        return false;
    ShowOkMsg(11);
    // 2. выдать сообщение об установке 60 В 1 А
    ShowMsg(12);
    Show1RetomDialog(1);
    // 3. получить аналоговые данные
    if (!Start7_3_2(12))
    {
        ShowErMsg(12);
        return false;
    }
    ShowOkMsg(12);
    // 4. ввести показания (с МИП или вручную)
    GetExternalData(12);
    // 5. рассчитать новые коэффициенты
    // 6. установить в конфигурации токи i2nom в 5 А и перезагрузить модуль
    // 2. выдать сообщение об установке 60 В 5 А
    // 3. получить аналоговые данные
    // 4. ввести показания (с МИП или вручную)
    // 5. рассчитать новые коэффициенты
}

void e_tunedialog::GetExternalData(int numexc)
{
    switch (TuneControlType)
    {
    case TUNEMIP:
    {
        StartMip();
        QTime tmr;
        tmr.start();
        while (tmr.elapsed() < 5000)
            qApp->processEvents();
        ShowOkMsg(numexc);
        StopMip();
        if (CheckMip())
        {
            for (int i=4; i<7; i++)
                mipd[i-4] = MipDat[i];
        }
        break;
    }
    case TUNEMAN:
    {
        QDialog *dlg = new QDialog(this);
        dlg->setObjectName("dlg7371");
        QGridLayout *glyout = new QGridLayout;
        QLabel *lbl = new QLabel("Введите значения сигналов по приборам");
        glyout->addWidget(lbl,0,0,1,6);
        for (int i=0; i<3; i++) // for A to C
        {
            lbl = new QLabel("Uф" + QString::number(i+10,36).toUpper()+", В");
            glyout->addWidget(lbl,1,i*2,1,1);
            QDoubleSpinBox *spb = new QDoubleSpinBox;
            spb->setDecimals(5);
            spb->setValue(60.0);
            spb->setObjectName("spb7371"+QString::number(i));
            glyout->addWidget(spb,1,i*2+1,1,1);
            QPushButton *pb = new QPushButton("Готово");
            connect(pb,SIGNAL(clicked()),this,SLOT(SetExtData()));
            glyout->addWidget(pb,2,0,1,6);
        }
        if (numexc == 12) // все параметры для п. 7.3.7.4
        {
            for (int i=0; i<3; i++) // for A to C
            {
                lbl = new QLabel("Iф" + QString::number(i+10,36).toUpper()+", A");
                glyout->addWidget(lbl,2,i*2,1,1);
                QDoubleSpinBox *spb = new QDoubleSpinBox;
                spb->setDecimals(5);
                spb->setValue(1.0);
                spb->setObjectName("spb7371"+QString::number(i+3));
                glyout->addWidget(spb,2,i*2+1,1,1);
            }
            for (int i=0; i<3; i++) // for A to C
            {
                lbl = new QLabel("Dф" + QString::number(i+10,36).toUpper()+", град");
                glyout->addWidget(lbl,3,i*2,1,1);
                QDoubleSpinBox *spb = new QDoubleSpinBox;
                spb->setDecimals(5);
                spb->setValue(0.0);
                spb->setObjectName("spb7371"+QString::number(i+6));
                glyout->addWidget(spb,3,i*2+1,1,1);
            }
            QPushButton *pb = new QPushButton("Готово");
            connect(pb,SIGNAL(clicked()),this,SLOT(SetExtData()));
            glyout->addWidget(pb,4,0,1,6);
        }
        dlg->setLayout(glyout);
        dlg->exec();
        ShowOkMsg(numexc);
    }
    case TUNERET:
    {
        for (int i=0; i<3; i++)
        {
            mipd[i] = 60.0;
            mipd[i+3] = 1.0;
        }
        ShowOkMsg(numexc);
        break;
    }
    }
}

void e_tunedialog::SetExtData()
{
    QDialog *dlg = this->findChild<QDialog *>("dlg7371");
    for (int i=0; i<3; i++)
    {
        QDoubleSpinBox *spb = this->findChild<QDoubleSpinBox *>("spb7371"+QString::number(i));
        mipd[i] = spb->value();
    }
    dlg->close();
}

// CHECKING

bool e_tunedialog::CheckTuneCoefs()
{
    double ValuesToCheck[26] = {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,\
                               0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.0};
    double ThresholdsToCheck[26] = {0.02,0.02,0.02,0.02,0.02,0.02,0.02,0.02,0.02,0.02,0.02,0.02,\
                                   0.02,0.02,0.02,0.02,0.02,0.02,1.0,1.0,1.0,1.0,1.0,1.0,0.02,0.005};
//    QLabel *lbl;
    bool res = true;
    for (int i = 0; i < 26; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("tune"+QString::number(i));
        if (lbl == 0)
            return false;
        bool ok;
        double vl = lbl->text().toDouble(&ok);
        if (!ok)
            return false;
        if (!IsWithinLimits(vl, ValuesToCheck[i], ThresholdsToCheck[i]))
        {
            res=false;
            lbl->setStyleSheet("QLabel {color: red};");
        }
    }
    return res;
}

bool e_tunedialog::IsWithinLimits(double number, double base, double threshold)
{
    float tmpf = fabs(number-base);
    if (tmpf<fabs(threshold))
        return true;
    else
        return false;
}

bool e_tunedialog::CheckMip()
{
    double ValuesToCheck[17] = {0,50.0,50.0,50.0,60.0,60.0,60.0,1.0,1.0,1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    double ThresholdsToCheck[17] = {0,0.1,0.1,0.1,1.0,1.0,1.0,0.05,0.05,0.05,0.1,0.1,0.1,0.1,0.1,0.1,0.1};
    for (int i = 1; i < 16; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("mip"+QString::number(i));
        if (lbl == 0)
            return false;
        bool ok;
        double vl = lbl->text().toDouble(&ok);
        if (!ok)
            return false;
        if (!IsWithinLimits(vl,ValuesToCheck[i],ThresholdsToCheck[i]))
            return false;
    }
    return true;
}

bool e_tunedialog::CheckAnalogValues(int ntest)
{
    double ValuesToCheck[44] = {25.0,50.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,\
                                0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,\
                                0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    double ThresholdsToCheck[44] = {25.0,0.05,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,\
                                    1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,\
                                    1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
    switch (ntest)
    {
    case 600: // test 60, 2U
    {
        break;
    }
    case 601: // test 60, 1I1U
    {
        for (int i = 5; i<8; i++)
        {
            ValuesToCheck[i] = ValuesToCheck[i+6] = 1.0; // i=1A
            ThresholdsToCheck[i] = ThresholdsToCheck[i+6] = 0.05; // +/- 0.05A
        }
        break;
    }
    case 602: // test 60, 2I
    {
        for (int i = 2; i<5; i++)
        {
            ValuesToCheck[i] = ValuesToCheck[i+3] = ValuesToCheck[i+6] = ValuesToCheck[i+9] = 1.0; // i=1A
            ThresholdsToCheck[i] = ThresholdsToCheck[i+3] = ThresholdsToCheck[i+6] = ThresholdsToCheck[i+9] = 0.05; // +/- 0.05A
            ValuesToCheck[i+18] = ValuesToCheck[i+30] = ValuesToCheck[i+24] = \
                    ValuesToCheck[i+36] = 60.0; // P=S=60Вт
            ThresholdsToCheck[i+18] = ThresholdsToCheck[i+30] = ThresholdsToCheck[i+24] = \
                    ThresholdsToCheck[i+36] = 1.5; // 2.5%
            ValuesToCheck[i+21] = ValuesToCheck[i+33] = 0.0; // Q=0ВАр
            ThresholdsToCheck[i+21] = ThresholdsToCheck[i+33] = 1.5; // 2.5%
            ValuesToCheck[i+27] = ValuesToCheck[i+39] = 1.0; // CosPhi=1.0
            ThresholdsToCheck[i+27] = ThresholdsToCheck[i+39] = 0.01;
        }
        break;
    }
    case 570:
    {
        for (int i = 2; i<8; i++)
        {
            ValuesToCheck[i] = ValuesToCheck[i+6] = 57.74; // u=57,74В
            ThresholdsToCheck[i] = ThresholdsToCheck[i+6] = 0.1; // +/- 0.1В
        }
        break;
    }
    case 571:
    {
        for (int i = 2; i<5; i++)
        {
            ValuesToCheck[i] = ValuesToCheck[i+6] = 57.74; // u=57,74В
            ThresholdsToCheck[i] = ThresholdsToCheck[i+6] = 0.1; // +/- 0.1В
            ValuesToCheck[i+3] = ValuesToCheck[i+9] = 1.0; // i=1A
            ThresholdsToCheck[i+3] = ThresholdsToCheck[i+9] = 0.05; // +/- 0.05A
        }
        break;
    }
    case 572:
    {
        for (int i = 2; i<5; i++)
        {
            ValuesToCheck[i] = ValuesToCheck[i+3] = ValuesToCheck[i+6] = ValuesToCheck[i+9] = 1.0; // i=1A
            ThresholdsToCheck[i] = ThresholdsToCheck[i+3] = ThresholdsToCheck[i+6] = ThresholdsToCheck[i+9] = 0.05; // +/- 0.05A
            ValuesToCheck[i+18] = ValuesToCheck[i+30] = ValuesToCheck[i+24] = \
                    ValuesToCheck[i+36] = 57.74; // P=S=60Вт
            ThresholdsToCheck[i+18] = ThresholdsToCheck[i+30] = ThresholdsToCheck[i+24] = \
                    ThresholdsToCheck[i+36] = 1.5; // 2.5%
            ValuesToCheck[i+21] = ValuesToCheck[i+33] = 0.0; // Q=0ВАр
            ThresholdsToCheck[i+21] = ThresholdsToCheck[i+33] = 1.5; // 2.5%
            ValuesToCheck[i+27] = ValuesToCheck[i+39] = 1.0; // CosPhi=1.0
            ThresholdsToCheck[i+27] = ThresholdsToCheck[i+39] = 0.01;
        }
        break;
    }
    }

    for (int i = 0; i < 44; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("value"+QString::number(i));
        if (lbl == 0)
            return false;
        bool ok;
        double vl = lbl->text().toDouble(&ok);
        if (!ok)
            return false;
        if (!IsWithinLimits(vl,ValuesToCheck[i],ThresholdsToCheck[i]))
            return false;
    }
    return true;
}

void e_tunedialog::ReadTuneCoefs()
{
    cn->Send(CN_Gac, &Bac_block, sizeof(Bac_block));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        RefreshTuneCoefs();
}

void e_tunedialog::WriteTuneCoefs()
{
    cn->Send(CN_Wac, &Bac_block, sizeof(Bac_block));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        QMessageBox::information(this,"Успешно!","Записано успешно!");
}

void e_tunedialog::CalcNewTuneCoefs()
{
/*    for (int i = 0; i < 16; i++)
    {
        Bac_block[i].fbin = 1.25 - (static_cast<float>(Bda0.sin[i])/(Z*1638.0));
        if ((Bda0.sin[i] == Bda5.sin[i]) || (Bda0.sin[i] == Bda20.sin[i]))
        {
            QMessageBox::warning(this,"Предупреждение","Ошибки в настроечных коэффициентах\nДеление на ноль");
            return;
        }
        Bac_block[i].fkuin = Z*1638.0/static_cast<float>(Bda0.sin[i]-Bda5.sin[i]);
        Bac_block[i].fkiin = Z*1638.0/static_cast<float>(Bda0.sin[i]-Bda20.sin[i]);
    }*/
}

void e_tunedialog::RefreshTuneCoefs()
{
    QLabel *lbl;
    for (int i = 0; i < 6; i++)
    {
        lbl = this->findChild<QLabel *>("tune"+QString::number(i));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bac_block.KmU[i], 'f', 5));
        lbl = this->findChild<QLabel *>("tune"+QString::number(i+6));
        if (lbl == 0)
            return;
        lbl->setText(QString("%1").arg(Bac_block.KmI_5[i], 0, 'f', 5));
        lbl = this->findChild<QLabel *>("tune"+QString::number(i+12));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bac_block.KmI_1[i], 'f', 5));
        lbl = this->findChild<QLabel *>("tune"+QString::number(i+18));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bac_block.DPsi[i], 'f', 5));
    }
    lbl = this->findChild<QLabel *>("tune24");
    if (lbl == 0)
        return;
    lbl->setText(QString::number(Bac_block.K_freq, 'f', 5));
    lbl = this->findChild<QLabel *>("tune25");
    if (lbl == 0)
        return;
    lbl->setText(QString::number(Bac_block.Kinter, 'f', 5));
}

void e_tunedialog::RefreshAnalogValues()
{
    QLabel *lbl;
    lbl = this->findChild<QLabel *>("value0");
    if (lbl == 0)
        return;
    lbl->setText(QString::number(Bda_block.Tmk, 'f', 5));
    lbl = this->findChild<QLabel *>("value1");
    if (lbl == 0)
        return;
    lbl->setText(QString::number(Bda_block.Frequency, 'f', 5));
    for (int i = 0; i < 6; i++)
    {
        lbl = this->findChild<QLabel *>("value"+QString::number(i+2));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.IUefNat_filt[i], 'f', 5));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+8));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.IUeff_filtered[i], 'f', 5));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+14));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.phi_next_f[i], 'f', 5));
    }
    for (int i=0; i<3; i++)
    {
        lbl = this->findChild<QLabel *>("value"+QString::number(i+20));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.PNatf[i], 'f', 5));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+23));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.SNatf[i], 'f', 5));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+26));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.QNatf[i], 'f', 5));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+29));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.CosPhiNat[i], 'f', 5));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+32));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.Pf[i], 'f', 5));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+35));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.Qf[i], 'f', 5));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+38));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.Sf[i], 'f', 5));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+41));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.CosPhi[i], 'f', 5));
    }
}

void e_tunedialog::StartAnalogMeasurements()
{
    tmr->start();
}

void e_tunedialog::StopAnalogMeasurements()
{
    tmr->stop();
}

void e_tunedialog::ReadAnalogMeasurements()
{
    // получение текущих аналоговых сигналов от модуля
    cn->Send(CN_GBdi, &Bda_block, sizeof(Bda_block));
    while (cn->Busy)
        qApp->processEvents();
    if (cn->result != CN_OK)
        return;
    // обновление коэффициентов в соответствующих полях на экране
    RefreshAnalogValues();
}

void e_tunedialog::LoadFromFile()
{
    QString filename = QFileDialog::getOpenFileName(this, "Открыть файл", ".", "Configuration (*.conf)");
    if (filename.isEmpty())
        return;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit error(ER_FILEOPENERROR);
        return;
    }
    QByteArray *ba = new QByteArray(file.readAll());
    publicclass::Bsi Bsi_block;
    if (ba->size() >= (sizeof(publicclass::Bsi)+sizeof(Bac_block)))
    {
        memcpy(&Bsi_block,ba,sizeof(publicclass::Bsi));
        if ((Bsi_block.CpuIdHigh != pc.CpuIdHigh) || (Bsi_block.SerNum != pc.SerNum))
        {
            if (QMessageBox::question(this,"Не тот файл","В файле содержатся данные для модуля с другим CPUID и/или SN.\nПродолжить загрузку?",\
                                      QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Ok);
            else
                return;
        }
        // продолжение загрузки файла
    }
}

void e_tunedialog::SaveToFile()
{

}

void e_tunedialog::StartMip()
{
    mipcanal = new iec104;
    if (MipErrNeeded)
        connect(mipcanal,SIGNAL(error(int)),this,SIGNAL(error(int)));
    connect(mipcanal,SIGNAL(signalsready(Parse104::Signals104&)),this,SLOT(MipData(Parse104::Signals104&)));
    connect(mipcanal,SIGNAL(ethconnected()),this,SLOT(EthConnected()));
    connect(mipcanal,SIGNAL(ethdisconnected()),this,SLOT(EthDisconnected()));
    connect(this,SIGNAL(stopall()),mipcanal,SLOT(Stop()));
    connect(mipcanal,SIGNAL(finished()),this,SLOT(DeleteMip()));
}

void e_tunedialog::EthConnected()
{
    QByteArray ba;
    ba.clear();
    ba.insert(0,0x3A);
    emit dataready(ba);
}

void e_tunedialog::EthDisconnected()
{
    QByteArray ba;
    ba.clear();
    ba.insert(0,0x3B);
    emit dataready(ba);
}

void e_tunedialog::MipData(Parse104::Signals104 &Signal)
{
    // приём из mipcanal::Signal номера сигнала (SigNum) и его значения (SigVal) и его дальнейшая обработка
    quint32 index = Signal.SigNum;
    if (index != -1)
    {
        QLabel *lbl = this->findChild<QLabel *>("mip"+QString::number(index));
        if (lbl != 0)
            lbl->setText(Signal.SigVal);
        MipDat[index] = Signal.SigVal.toFloat();
    }
}

void e_tunedialog::StopMip()
{
    emit stopall();
}

void e_tunedialog::DeleteMip()
{
    delete mipcanal;
}

void e_tunedialog::closeEvent(QCloseEvent *e)
{
    emit stopall();
    e->accept();
}

void e_tunedialog::ShowMsg(int msg)
{
    QLabel *lbl = this->findChild<QLabel *>("tunemsg"+QString::number(msg));
    if (lbl == 0)
        return;
    lbl->setVisible(true);
}

void e_tunedialog::ShowOkMsg(int msg)
{
    QLabel *lbl=this->findChild<QLabel *>("tunemsgres"+QString::number(msg));
    if (lbl == 0)
        return;
    lbl->setText("готово!");
    lbl->setVisible(true);
}

void e_tunedialog::ShowErMsg(int msg)
{
    QLabel *lbl=this->findChild<QLabel *>("tunemsgres"+QString::number(msg));
    if (lbl == 0)
        return;
    lbl->setStyleSheet("QLabel {color: red};");
    lbl->setText("ошибка!");
    lbl->setVisible(true);
}
