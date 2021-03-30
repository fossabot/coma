#include "confkivdialog.h"

#include "../config/configkiv.h"
#include "../gen/board.h"
#include "../gen/error.h"
#include "../gen/s2.h"
#include "../widgets/basespinboxgroup.h"
#include "../widgets/wd_func.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>

ConfKIVDialog::ConfKIVDialog(ConfigKIV *ckiv, QWidget *parent) : AbstractConfDialog(parent)
{
    m_list = { CType, Abs_104, Cycle_104, T1_104, T2_104, T3_104, k_104, w_104, IP_ID, Mask_ID, GW_ID, Port_ID, SNTP_ID,
        Baud_ID, Parity_ID, stopbit_ID, adrMB_ID, NFiltr_ID, Unom1, DUImin_ID, Imin, dC_pred, dC_alarm, dTg_pred,
        dTg_alarm, dIunb_pred, dIunb_alarm, GdC, GdTg, GdIunb, RTerm, W100, T_Data_Rec, LowU, IsU, IsIuIunb,
        Tevent_pred, Tevent_alarm, Tg_Pasp_ID, C_Pasp_ID, U2nom, MBMaster, MBMab1, MBMab2, MBMab3, MBMab4 };
    CKIV = ckiv;
    CKIV->KxxConfig()->setParent(this);
    CKIV->MainConfig()->setParent(this);
}

ConfKIVDialog::~ConfKIVDialog()
{
    delete CKIV;
}

void ConfKIVDialog::Fill()
{
    CKIV->MainConfig()->Fill();
    CKIV->KxxConfig()->Fill();
    FillKiv();
}

void ConfKIVDialog::FillBack() const
{
    WidgetFactory factory;
    for (const auto id : m_list)
    {
        bool status = factory.fillBack(id, this);
        if (!status)
        {
            qWarning() << "Couldnt fill back item from widget: " << id;
        }
    }
    // CKIV->MainConfig()->FillBack();
    //  CKIV->KxxConfig()->FillBack();
    // FillBackKiv();
}

QWidget *ConfKIVDialog::analogWidget()
{
    QWidget *w = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    gridlyout->setAlignment(Qt::AlignVCenter);

    const QStringList phases { "Фаза A:", "Фаза B:", "Фаза C:" };

    int row = 0;
    QGroupBox *gb = new QGroupBox("Аналоговые параметры");

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное линейное первичное напряжение, кВ:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Unom1), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное вторичное напряжение первой тройки, В:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::U2nom), 0, 10000, 1), row, 2, 1, 3);
    row++;

    QHBoxLayout *hlyout = new QHBoxLayout;

    for (const auto &phase : phases)
    {
        hlyout->addWidget(new QLabel(phase, this));
    }
    gridlyout->addLayout(hlyout, row, 2, 1, 1, Qt::AlignTop);

    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Паспортные значения ёмкости вводов, пФ:"), row, 1, 1, 1, Qt::AlignTop);

    gridlyout->addWidget(
        WDFunc::NewSPBG(this, nameByValue(BciNumber::C_Pasp_ID), 3, 0, 20000, 1), row, 2, 1, 1, Qt::AlignTop);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Паспортные значения tg вводов, %:"), row, 1, 1, 1);

    gridlyout->addWidget(WDFunc::NewSPBG(this, nameByValue(BciNumber::Tg_Pasp_ID), 3, 0, 10, 2), row, 2, 1, 1);

    row++;

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    w->setLayout(lyout);
    return w;
}

QWidget *ConfKIVDialog::thresholdsWidget()
{
    QWidget *w = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;

    int row = 0;
    QGroupBox *gb = new QGroupBox("Уставки сигнализации");

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка предупредительной сигнализации по изменению емкости, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::dC_pred), 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка аварийной сигнализации по изменению емкости, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::dC_alarm), 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка предупредительной сигнализации по изменению tg δ, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::dTg_pred), 0, 1000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Уставка аварийной сигнализации по изменению tg δ, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::dTg_alarm), 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Уставка предупредительной сигнализации по "
                             "изменению небаланса токов, %:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::dIunb_pred), 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка аварийной сигнализации по изменению небаланса токов, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::dIunb_alarm), 0, 10000, 1), row, 2, 1, 3);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Уставки контроля минимума тока и напряжения");
    gridlyout = new QGridLayout;

    row = 0;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка контроля минимума напряжения (в % от номинального):"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::DUImin_ID), 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Уставка контроля минимума тока (в % от Imax):"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Imin), 0, 10000, 1), row, 2, 1, 3);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
}

QWidget *ConfKIVDialog::remainsWidget()
{
    QWidget *w = new QWidget;
    QGroupBox *gb = new QGroupBox("Гистерезис");

    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    int row = 0;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Гистерезис на отключение сигнализации по dC, % от уставки:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::GdC), 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Гистерезис на отключение сигнализации по dTg, % от уставки:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::GdTg), 0, 10000, 3), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Гистерезис на отключение сигнализации по небалансу токов, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::GdIunb), 0, 10000, 1), row, 2, 1, 3);
    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    //............................................................

    gb = new QGroupBox("Сигнализации событий");
    gridlyout = new QGridLayout;

    row = 0;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Низкое напряжение для сигнализации:"), row, 0, 1, 1, Qt::AlignLeft);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::LowU), 0, 10000, 1), row, 1, 1, 1);

    gridlyout->addWidget(
        WDFunc::NewChB2(this, nameByValue(BciNumber::IsU), "Сигнализация по наличию входного напряжения, % от ном."),
        row, 2, 1, 1);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Задержка на формирование предупредительных событий, сек:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Tevent_pred), 0, 10000, 1), row, 1, 1, 1);

    gridlyout->addWidget(
        WDFunc::NewChB2(this, nameByValue(BciNumber::IsIuIunb), "Сигнализация по небалансу токов"), row, 2, 1, 1);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Задержка на формирование аварийных событий, сек:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Tevent_alarm), 0, 10000, 1), row, 1, 1, 1);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("");
    gridlyout = new QGridLayout;

    row = 0;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Интервал усреднения данных  (в периодах основной частоты):"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::NFiltr_ID), 0, 10000, 0), row, 1, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Интервал записи данных в ПЗУ (тренд), в секундах:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::T_Data_Rec), 0, 10000, 0), row, 1, 1, 3);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
}

QWidget *ConfKIVDialog::connectionWidget()
{
    QWidget *w = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;

    gb->setTitle("Настройки протокола МЭК-60870-5-104");

    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(CKIV->MainConfig()->MainWidget(this));
    hlyout->addWidget(CKIV->KxxConfig()->ComParam(this));

    gb->setLayout(hlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Настройка времени");

    vlyout->addWidget(CKIV->MainConfig()->TimeWidget(this));

    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    w->setLayout(lyout);
    return w;
}

QWidget *ConfKIVDialog::testWidget()
{
    QGroupBox *groupBox = new QGroupBox(this);
    QGridLayout *vlyout = new QGridLayout;
    //  QVBoxLayout *subvlyout = new QVBoxLayout;
    WidgetFactory factory;
    for (const auto i : (m_list))
    {
        QWidget *widget = factory.createWidget(i, this);
        if (!widget)
        {
            qWarning() << "Bad config widget for item: " << QString::number(i);
            continue;
        }
        int group = i / 10;
        // if (widget)
        //  {

        //            for (const auto *i : widget->findChildren<QWidget *>())
        //            {
        //                qDebug() << i->metaObject()->className() << i->sizePolicy().horizontalPolicy();
        //            }
        QLayoutItem *child = vlyout->itemAtPosition(group, 0);
        QGroupBox *subBox = nullptr;
        if (!child)
        {
            subBox = new QGroupBox("Группа " + QString::number(group), this);
            subBox->setLayout(new QVBoxLayout);
        }
        else
        {
            subBox = qobject_cast<QGroupBox *>(child->widget());
            vlyout->removeWidget(subBox);
        }
        // if (!subBox)
        // {
        // sublyout = new QVBoxLayout;
        //  subBox = new QGroupBox(this);

        //  vlyout->addLayout(sublyout);
        // }
        QLayout *lyout = subBox->layout();
        lyout->addWidget(widget);
        subBox->setLayout(lyout);
        vlyout->addWidget(subBox, group, 0);
        // }
        //        else
        //        {
        //            WidgetFactory::createItem(i, this);
        //        }
    }
    //    for (quint32 i = 0; i != 10; ++i)
    //    {
    //        QWidget *widget = WidgetFactory::createWidget(static_cast<BciNumber>(i), this);
    //        if (widget)
    //            vlyout->addWidget(widget);
    //    }
    groupBox->setLayout(vlyout);
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(groupBox);
    return scrollArea;
}

void ConfKIVDialog::FillKiv()
{
    using namespace DataTypes;
    SetSPBData<float>(this, BciNumber::Unom1);
    SetSPBData<float>(this, BciNumber::DUImin_ID);
    SetSPBData<float>(this, BciNumber::Imin);

    const auto tg_pasp = S2::getRecord(BciNumber::Tg_Pasp_ID).value<FLOAT_3t>();
    WDFunc::SetSPBGData(this, nameByValue(BciNumber::Tg_Pasp_ID), tg_pasp);

    const auto c_pasp = S2::getRecord(BciNumber::C_Pasp_ID).value<FLOAT_3t>();
    WDFunc::SetSPBGData(this, nameByValue(BciNumber::C_Pasp_ID), c_pasp);

    SetSPBData<float>(this, BciNumber::dC_pred);
    SetSPBData<float>(this, BciNumber::dC_alarm);
    SetSPBData<float>(this, BciNumber::dTg_pred);
    SetSPBData<float>(this, BciNumber::dTg_alarm);
    SetSPBData<float>(this, BciNumber::dIunb_pred);
    SetSPBData<float>(this, BciNumber::dIunb_alarm);

    //// Значения не записываются
    SetSPBData<float>(this, BciNumber::GdC);
    SetSPBData<float>(this, BciNumber::GdTg);
    SetSPBData<float>(this, BciNumber::GdIunb);
    ////

    SetSPBData<DWORD>(this, BciNumber::NFiltr_ID);
    SetSPBData<DWORD>(this, BciNumber::T_Data_Rec);

    SetSPBData<float>(this, BciNumber::U2nom);
    SetSPBData<float>(this, BciNumber::LowU);

    SetSPBData<DWORD>(this, BciNumber::Tevent_pred);
    SetSPBData<DWORD>(this, BciNumber::Tevent_alarm);

    WDFunc::SetChBData(this, nameByValue(BciNumber::IsU), bool(S2::getRecord(BciNumber::IsU).value<BYTE>()));
    WDFunc::SetChBData(this, nameByValue(BciNumber::IsIuIunb), bool(S2::getRecord(BciNumber::IsIuIunb).value<BYTE>()));
}

void ConfKIVDialog::FillBackKiv() const
{
    using namespace DataTypes;
    SPBDataS2<float>(this, BciNumber::Unom1);
    SPBDataS2<float>(this, BciNumber::DUImin_ID);
    SPBDataS2<float>(this, BciNumber::Imin);

    FLOAT_3t tg_pasp;
    WDFunc::SPBGData(this, nameByValue(BciNumber::Tg_Pasp_ID), tg_pasp);
    S2::setRecordValue({ BciNumber::Tg_Pasp_ID, tg_pasp });

    FLOAT_3t c_pasp;
    WDFunc::SPBGData(this, nameByValue(BciNumber::C_Pasp_ID), c_pasp);
    S2::setRecordValue({ BciNumber::C_Pasp_ID, c_pasp });

    SPBDataS2<float>(this, BciNumber::dC_pred);
    SPBDataS2<float>(this, BciNumber::dC_alarm);
    SPBDataS2<float>(this, BciNumber::dTg_pred);
    SPBDataS2<float>(this, BciNumber::dTg_alarm);
    SPBDataS2<float>(this, BciNumber::dIunb_pred);
    SPBDataS2<float>(this, BciNumber::dIunb_alarm);

    SPBDataS2<DWORD>(this, BciNumber::NFiltr_ID);
    SPBDataS2<DWORD>(this, BciNumber::T_Data_Rec);

    SPBDataS2<float>(this, BciNumber::U2nom);
    SPBDataS2<float>(this, BciNumber::LowU);

    SPBDataS2<DWORD>(this, BciNumber::Tevent_pred);
    SPBDataS2<DWORD>(this, BciNumber::Tevent_alarm);

    S2::setRecordValue({ BciNumber::IsU, BYTE(WDFunc::ChBData(this, nameByValue(BciNumber::IsU))) });
    S2::setRecordValue({ BciNumber::IsIuIunb, BYTE(WDFunc::ChBData(this, nameByValue(BciNumber::IsIuIunb))) });
}

void ConfKIVDialog::SetupUI()
{

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");

    // ConfTW->addTab(analogWidget(), "Аналоговые");

    //  ConfTW->addTab(thresholdsWidget(), "Уставки");

    //   ConfTW->addTab(remainsWidget(), "Остальное");

    //  ConfTW->addTab(connectionWidget(), "Связь");

    //  ConfTW->addTab(CKIV->KxxConfig()->ModbusWidget(this), "ModBusMaster");
    // ConfTW->addTab(CKIV->KxxConfig()->VariousWidget(this), "Общее");

    ConfTW->addTab(testWidget(), "Work in progress");

    lyout->addWidget(ConfTW);
    lyout->addWidget(ConfButtons());
    setLayout(lyout);
}

void ConfKIVDialog::CheckConf() const
{
}

void ConfKIVDialog::SetDefConf()
{
    CKIV->setDefConf();

    Fill();
}
