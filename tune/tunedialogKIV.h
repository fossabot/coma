#ifndef TuneDialogKIV_H
#define TuneDialogKIV_H

#include "../check/checkkiv.h"
#include "../config/config.h"
#include "../config/configkiv.h"
#include "../gen/modulebsi.h"
#include "../iec104/iec104.h"
#include "eabstracttunedialog.h"
#include "limereport/lrreportengine.h"
#include "tunekiv.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QStandardItemModel>

#define TUNEFILELENGTH 256

// voltages
#define V60 60.0
#define V57 57.74

// frequencies
#define HZ50 50.0

// currents
#define I1 1.0
#define I5 5.0

#define C5012 5012
#define C10024 10024
#define C15036 15036
#define C2506 2506
#define C1253 1253
#define C626 626

// parameters for GetExtData
#define TD_GED_U 0x01 // напряжение
#define TD_GED_I 0x02 // ток
#define TD_GED_F 0x04 // частота
#define TD_GED_D 0x08 // угол нагрузки

#define TD_TMK 25.0 // degrees
#define TD_VBAT 3.0 // voltage
#define TD_FREQ 50 // Hz
#define CONST2PIF 314.15926

class TuneDialogKIV : public QDialog
{
    Q_OBJECT
public:
    explicit TuneDialogKIV(QVector<S2::DataRec> *S2Config, QWidget *parent = nullptr);

signals:
    void Send();

public slots:

private:
    ConfigKIV::Bci m_Bci_block_work, m_Bci_block_temporary;
    ConfigKIV *CKIV;

    QVector<S2::DataRec> *S2Config;
    QDialog *ask;
    QLineEdit *ledit;
    QTimer *timer;

    float m_Uet[3], m_Iet[3], m_PHIet[6], m_FREQet, m_UetMinus20[3], m_IetMinus20[3], m_PHIetMinus20[6],
        m_FREQetMinus20, m_Uet60[3], m_Iet60[3], m_PHIet60[6], m_FREQet60;

    enum TuneModes
    {
        TUNEMIP,
        TUNERET,
        TUNEMAN
    };

    LimeReport::ReportEngine *m_Report;

    int m_tuneControlType;
    bool m_Cancelled = false, m_defConfig;

    void SetupUI();

    struct Bd0
    {
        float Tmk; // Температура кристалла микроконтроллера, °С
        float Vbat; // Напряжение аккумуляторной батареи, В
    };

    Bd0 m_Bd_block20, m_Bd_block_minus20, m_Bd_block_plus60;
    int m_filterSteps;

    /*struct ReportHeaderStructure
    {
        QString Organization;   // организация, проводившая проверку
        QString Day;            // день месяца проведения проверки
        QString Month;          // месяц
        QString Yr;             // две последние цифры года (20хх)
        QString Freq;           // обозначение частоты
        QString UA;             // напряжение фазы А
        QString UB;             // напряжение фазы B
        QString UC;             // напряжение фазы С
        QString IA;             // ток фазы А
        QString IB;             // ток фазы B
        QString IC;             // ток фазы C
        QString PhiloadA;       // угол нагрузки фазы А
        QString PhiloadB;       // угол нагрузки фазы B
        QString PhiloadC;       // угол нагрузки фазы C
        QString PhiUAB;         // угол между напряжениями фаз А и B
        QString PhiUBC;         // угол между напряжениями фаз B и C
        QString OffsetF;        // Погрешности
        QString OffsetUA;
        QString OffsetUB;
        QString OffsetUC;
        QString OffsetIA;
        QString OffsetIB;
        QString OffsetIC;
        QString OffsetPhiloadA;
        QString OffsetPhiloadB;
        QString OffsetPhiloadC;
        QString OffsetPhiUAB;
        QString OffsetPhiUBC;
    };

    QStringList TableItem;      // строка таблицы с данными вывода
    QList<QStringList *> MainData; // полные данные таблицы для модели

    ReportHeaderStructure ReportHeader;


    int Index, Counter;
    float CurrentS; // текущее значение нагрузки
    int PovType, TempPovType; // тип поверяемого оборудования (по какому ГОСТу)
    QStandardItemModel *ReportModel, *ViewModel; // модель, в которую заносим данные для отчёта
    int RowCount, ColumnCount; // количество рядов и столбцов в модели*/

    /*double U[21] =       {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 72, 48, 36, 24, 12, 60, 60, 60, 60 };
    double I[21] =       {0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1, 2, 3, 4, 5, 6,  5, 5, 5, 5, 5, 5, 5, 5, 5 };
    double PhiLoad[21] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 90, 180, 270 };*/

    RealDataStruct m_RealData;
    float m_IUefNat_filt_old[6]; // для сохранения значений по п. 7.3.2
    float m_MipDat[46];
    int m_GED_Type;
    int m_Kadc;

    // QHBoxLayout *MipPars(int parnum, const QString &groupname);
    void FillBac(int bacnum);
    void FillBackBac(int bacnum);
    void PrepareConsts();
    void FillNewBac();

    void SetLbls();
    void SetPf();
    void Tune3p();
    int CheckTuneCoefs();
    // int CheckMip();
    void GetBdAndFill();
    int Start7_2_1();
    int Start_7_2_7();
    int Start7_3_1();
    int Start7_3_1_1();
    int Start7_3_2();
    int Start7_3_4();
    int Start7_3_4_2();
    int Start7_3_4_6();
    int Start7_3_4_7();
    int Start7_3_4_11();
    int Start7_3_4_12();
    int Start7_3_4_13();
    int Start7_3_4_14();
    int Start7_3_4_15();
    int Start7_3_4_16();
    int Start7_3_4_17();
    int Start7_3_4_18();
    int Start7_3_5();
    int ShowScheme();
    int SaveUeff();
    int ShowRetomDialog(double U, double I, double Y);
    int SetNewTuneCoefs(); // заполнение Bac_newblock, чтобы не было пурги после настройки
    int SaveWorkConfig();
    int LoadWorkConfig();
    QWidget *Bd1W(QWidget *parent);
    void FillBd1(QWidget *parent);
    void RefreshAnalogValues(int bdnum);
    int Show80();
    int Show120();
    void CalcNewPt100Coefs();
    void CalcTempCorCoefs();
    void EnterDataTune();
    int Show125mA();
    int AskTemp20();
    int AskTempMinus20();
    int AskTemp60();
    int Average();
    int ReadAnalogTemp20();
    int ReadAnalogTempMinus20();
    int ReadAnalogTemp60();
    void Enter20Data();
    void Enterminus20Data();
    void Enter60Data();
    float ToFloat(QString text);

    QString ValuesFormat, WidgetFormat;

private slots:

    int TuneChannel();
    void ReadN();
    int ReadAnalogMeasurements();
    void SetExtData();
    void CancelExtData();
    void CancelTune();
    void GenerateReport();
    int TunePt100Channel();
    int TuneTemp();
    int TuneTempCor();
    int CalcTuneCoefs();
    void SaveValuesTemp20();
    void SaveValuesTempMinus20();
    void SaveValuesTemp60();
    void CloseAsk();

    void SetDefCoefs();
};

#endif // TUNEDIALOG85_H
