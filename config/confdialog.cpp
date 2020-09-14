#include "confdialog.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../widgets/ecombobox.h"
//#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVBoxLayout>

ConfDialog::ConfDialog(QVector<S2::DataRec> *S2Config, quint32 MTypeB, quint32 MTypeM, QWidget *parent)
    : QWidget(parent)
{
    ConfigMain = new Config(S2Config, MTypeB, MTypeM); // добавляем к переданному S2Config общую часть
                                                       // SetupUI();

    ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; "
                   "padding: 1px; color: black;"
                   "background-color: "
        + QString(Colors::ACONFOCLR) + "; font: bold 10px;}";
    WidgetFormat = "QWidget {background-color: " + QString(Colors::ACONFWCLR) + ";}";
    // QString tmps = "QWidget {background-color: " + QString(Colors::ACONFWCLR) + ";}";
}

QWidget *ConfDialog::SetupMainBlk(QObject *parent)
{
    ParentMainbl = parent;
    QWidget *w = new QWidget;
    QString paramcolor = Colors::MAINWINCLR;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);
    QLabel *lbl;
    QDoubleSpinBox *dspbls;

    lbl = new QLabel("Адрес базовой станции:");
    glyout->addWidget(lbl, 0, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "Abs_104", 0, 65535, 0, paramcolor);
    glyout->addWidget(dspbls, 0, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Интервал циклического опроса, с:");
    glyout->addWidget(lbl, 1, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "Cycle_104", 0, 255, 0, paramcolor);
    glyout->addWidget(dspbls, 1, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Тайм-аут t1, с:");
    glyout->addWidget(lbl, 2, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "T1_104", 0, 255, 0, paramcolor);
    glyout->addWidget(dspbls, 2, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Тайм-аут t2, с:");
    glyout->addWidget(lbl, 3, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "T2_104", 0, 255, 0, paramcolor);
    glyout->addWidget(dspbls, 3, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Тайм-аут t3, с:");
    glyout->addWidget(lbl, 4, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "T3_104", 0, 255, 0, paramcolor);
    glyout->addWidget(dspbls, 4, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Макс. число неподтв. APDU (k):");
    glyout->addWidget(lbl, 5, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "k_104", 0, 255, 0, paramcolor);
    glyout->addWidget(dspbls, 5, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Макс. число посл. подтв. APDU (w):");
    glyout->addWidget(lbl, 6, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "w_104", 0, 255, 0, paramcolor);
    glyout->addWidget(dspbls, 6, 1, 1, 1, Qt::AlignLeft);

    vlyout2->addLayout(glyout);

    w->setLayout(vlyout2);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *ConfDialog::SetupTime(QObject *parent)
{
    ParentCtype = parent;
    QString paramcolor = Colors::MAINWINCLR;
    QWidget *w = new QWidget;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    QFont font;
    font.setFamily("Times");
    font.setPointSize(11);

    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);
    int row = 0;
    QLabel *lbl;

    lbl = new QLabel("Тип синхронизации времени:");

    glyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);

    QStringList cbl = QStringList { "SNTP+PPS", "SNTP" };
    EComboBox *cb = WDFunc::NewCB(this, "Ctype", cbl, paramcolor);
    glyout->addWidget(cb, row, 1, 1, 1);

    vlyout2->addLayout(glyout);

    w->setLayout(vlyout2);
    w->setStyleSheet(WidgetFormat);
    return w;
}

void ConfDialog::Fill()
{
    int cbidx;
    WDFunc::SetSPBData(ParentMainbl, "Abs_104", ConfigMain->MainBlk.Abs_104);
    WDFunc::SetSPBData(ParentMainbl, "Cycle_104", ConfigMain->MainBlk.Cycle_104);
    WDFunc::SetSPBData(ParentMainbl, "T1_104", ConfigMain->MainBlk.T1_104);
    WDFunc::SetSPBData(ParentMainbl, "T2_104", ConfigMain->MainBlk.T2_104);
    WDFunc::SetSPBData(ParentMainbl, "T3_104", ConfigMain->MainBlk.T3_104);
    WDFunc::SetSPBData(ParentMainbl, "k_104", ConfigMain->MainBlk.k_104);
    WDFunc::SetSPBData(ParentMainbl, "w_104", ConfigMain->MainBlk.w_104);

    cbidx = ((ConfigMain->MainBlk.Ctype & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(ParentCtype, "Ctype", cbidx);
}

void ConfDialog::FillBack()
{
    WDFunc::SPBData(ParentMainbl, "Abs_104", ConfigMain->MainBlk.Abs_104);
    WDFunc::SPBData(ParentMainbl, "Cycle_104", ConfigMain->MainBlk.Cycle_104);
    WDFunc::SPBData(ParentMainbl, "T1_104", ConfigMain->MainBlk.T1_104);
    WDFunc::SPBData(ParentMainbl, "T2_104", ConfigMain->MainBlk.T2_104);
    WDFunc::SPBData(ParentMainbl, "T3_104", ConfigMain->MainBlk.T3_104);
    WDFunc::SPBData(ParentMainbl, "k_104", ConfigMain->MainBlk.k_104);
    WDFunc::SPBData(ParentMainbl, "w_104", ConfigMain->MainBlk.w_104);

    int cbidx = WDFunc::CBIndex(ParentMainbl, "Ctype");
    cbidx = ((cbidx & 0x01) ? 1 : 0);
    ConfigMain->MainBlk.Ctype = (0x00000001 << cbidx) - 1;
}

void ConfDialog::SetDefConf()
{
    ConfigMain->SetDefBlock();
    // Fill();
}
