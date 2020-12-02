#pragma once
#include "../gen/pch.h"

#include <QMap>
/**
 *  Перечисление для хранения списка приборов
 */
namespace Modules
{

Q_NAMESPACE
enum Model
{
    KTF = 0xA287,
    KIV = 0xA284,
    KDV = 0xA387
};

Q_ENUM_NS(Model);
enum BaseBoards
{
    MTB_00 = 0x0000,
    MTB_12 = 0x1200,
    MTB_21 = 0x2100,
    MTB_22 = 0x2200,
    MTB_31 = 0x3100,
    MTB_35 = 0x3500,
    MTB_80 = 0x8000,
    MTB_81 = 0x8100,
    MTB_84 = 0x8400,
    MTB_85 = 0x8500,
    MTB_87 = 0x8700,
    MTB_A1 = 0xA100,
    MTB_A2 = 0xA200
};

enum MezzanineBoards
{
    MTM_00 = 0x0000,
    MTM_21 = 0x0021,
    MTM_22 = 0x0022,
    MTM_31 = 0x0031,
    MTM_35 = 0x0035,
    MTM_81 = 0x0081,
    MTM_82 = 0x0082,
    MTM_83 = 0x0083,
    MTM_84 = 0x0084,
    MTM_85 = 0x0085
};

static const QMap<quint16, QString> BaseBoards {
    { MTB_00, "АВ_ТУК-00" }, // нет базовой платы
    //        {MTB_12,{"АВ-ТУК-12 (ЦКУ)", 0x1200}}, // процессорный, не комбинируется
    { MTB_21, "АВ-ТУК-21" },     // аналоговый ввод DC, комбинируется с 2x,3x
    { MTB_22, "АВ-ТУК-22" },     // аналоговый ввод DC, комбинируется с 2x,3x
    { MTB_31, "АВ-ТУК-31" },     // дискретный ввод, комбинируется с 2x,3x
    { MTB_35, "АВ-ТУК-35" },     // дискретный вывод, комбинируется с 2x,3x
    { MTB_80, "АВТУК-" },        // аналоговый ввод AC, комбинируется с 81,82,83
    { MTB_81, "АВ-ТУК-81" },     // МНК3, комбинируется с 84
    { MTB_84, "АВ-ТУК-84" },     // НКВВ, комбинируется с 81,82,83
    { MTB_85, "АВ-ТУК-85" },     // УСК, комбинируется с 85
    { MTB_87, "АВ-ТУК-87 (ЧР)" } // ЧР, не комбинируется
    //        {MTB_A1,{"ПКС-1", 0xA100}}, // ПКДН, прибор для Туркота, не комбинируется
    //        {MTB_A2,{"АВМ", 0xA200}}, // МНК3, не комбинируется
};

static const QMap<quint16, QString> MezzanineBoards {
    { MTM_00, "" },           // нет мезонина
    { MTM_21, "21" },         // аналоговый ввод, комбинируется с 2x,3x
    { MTM_22, "22" },         // аналоговый ввод, комбинируется с 2x,3x
    { MTM_31, "31" },         // дискретный ввод, комбинируется с 2x,3x
    { MTM_35, "35" },         // дискретный вывод, комбинируется с 2x,3x
    { MTM_81, "81 (Э2Т0Н)" }, // 0U6I, комбинируется с 80,84
    { MTM_82, "82 (Э1Т1Н)" }, // 3U3I, комбинируется с 80,84
    { MTM_83, "83 (Э0Т2Н)" }, // 6U0I, комбинируется с 80,84
    { MTM_84, "84" },         // 3U3I, комбинируется с 80,81
    { MTM_85, "85 (УСК)" }    // 3U3I УСК (перегрузка по току 20х), комбинируется с 85
};

struct StartupInfoBlock
{
    quint32 MTypeB;
    quint32 MTypeM;
    quint32 HwverB;
    quint32 Fwver;
    quint32 Rst;
    quint32 RstCount;
    quint32 UIDLow;
    quint32 UIDMid;
    quint32 UIDHigh;
    quint32 SerialNumB;
    quint32 SerialNumM;
    quint32 HwverM;
    quint32 SerialNum;
    quint32 Cfcrc;
    quint32 Hth;
};

struct StartupInfoBlockExt
{
};

}
#define MAXERRORFLAGNUM 32

#define HTH_FNC 0x00000200     // частота не в допуске (Э)
#define HTH_LS 0x00000100      // сигналы малы (Э)
#define HTH_CONFIG 0x00000080  // в модуле нет конфигурации
#define HTH_REGPARS 0x00000040 // в модуле нет настроечных параметров
#define HTH_ADC 0x00000020     // проблема с АЦП (Э)
#define HTH_1PPS 0x00000010    // нет сигнала 1PPS
#define HTH_REL 0x00000008     // неисправность выходных реле (Д)
#define HTH_TUPP 0x00000004    // перегрев модуля

/// Bci
namespace Bci
{
Q_NAMESPACE
// enum class BciMainEnum : byte
//{
//    MTypeB = 1,
//    MTypeM = 2,
//    Ctype = 3,
//    Abs_104 = 4,
//    Cycle_104 = 5,
//    T1_104 = 6,
//    T2_104 = 7,
//    T3_104 = 8,
//    K_104 = 9,
//    W_104 = 10
//};
// Q_ENUM_NS(Bci::BciMainEnum);

// enum class BciDefMainValues : byte
//{
//    MTypeB = 0,
//    MTypeM = 0,
//    Ctype = 1,
//    Abs_104 = 205,
//    Cycle_104 = 5,
//    T1_104 = 15,
//    T2_104 = 10,
//    T3_104 = 20,
//    K_104 = 12,
//    W_104 = 8
//};
// Q_ENUM_NS(Bci::BciDefMainValues);
constexpr int DEF_CTYPE = 1;
constexpr int DEF_ABS_104 = 205;
constexpr int DEF_CYCLE_104 = 5;
constexpr int DEF_T1_104 = 15;
constexpr int DEF_T2_104 = 10;
constexpr int DEF_T3_104 = 20;
constexpr int DEF_K_104 = 12;
constexpr int DEF_W_104 = 8;
struct BciMain
{
    BciMain()
    {
        MTypeB = Modules::MTB_00;
        MTypeM = Modules::MTM_00;
        Ctype = DEF_CTYPE;
        Abs_104 = DEF_ABS_104;
        Cycle_104 = DEF_CYCLE_104;
        T1_104 = DEF_T1_104;
        T2_104 = DEF_T2_104;
        T3_104 = DEF_T3_104;
        K_104 = DEF_K_104;
        W_104 = DEF_W_104;
    }
    quint32 MTypeB;
    quint32 MTypeM;
    // Тип синхронизации времени от модуля Ц
    quint32 Ctype;
    // Адрес базовой станции для протокола 104
    quint32 Abs_104;
    // Интервал циклического опроса по протоколу МЭК 60870-5-104
    quint32 Cycle_104;
    // тайм-аут Т1 для протокола 104
    quint32 T1_104;
    // тайм-аут Т2 для протокола 104
    quint32 T2_104;
    // тайм-аут Т3 для протокола 104
    quint32 T3_104;
    // макс. кол-во неподтв. сообщений
    quint32 K_104;
    // макс. кол-во сообщений, после которых необх. выдать подтверждение
    quint32 W_104;
};
}

constexpr int BCI_MTYPEB = 1;
constexpr int BCI_MTYPEM = 2;
constexpr int BCI_CTYPE = 3;
constexpr int BCI_ABS_104 = 4;
constexpr int BCI_CYCLE_104 = 5;
constexpr int BCI_T1_104 = 6;
constexpr int BCI_T2_104 = 7;
constexpr int BCI_T3_104 = 8;
constexpr int BCI_K_104 = 9;
constexpr int BCI_W_104 = 10;

/// Bci

namespace AVM
{
const QStringList sysJourDescription {
    "Рестарт модуля",                                                    //
    "Произошла запись и переход на новую конфигурацию",                  //
    "Произошла запись и переход на новую версию ВПО",                    //
    "Произошла запись в блок Bhb (hidden block)",                        //
    "Произошла запись в блок Bhbm (hidden block)( Мезонин)",             //
    "Произошёл отказ внешней Flash-памяти 4 Мбайт на базовой плате",     //
    "Произошёл отказ внешней Flash-памяти 512 байт на мезонинной плате", //
    "Ошибка работы внешней FRAM памяти",                                 //
    "Произошёл отказ при обновлении конфигурации",                       //
    "Ошибка загрузки конфигурации из flash памяти",                      //
    "Ошибка регулировочных коэффициентов (базовая)",                     //
    "Ошибка регулировочных коэффициентов (Мезонин)",                     //
    "Ошибка перегрев модуля",                                            //
    "Напряжение батарейки низкое (< 2,5 В)",                             //
    "Ошибка связи с NTP сервером",                                       //
    "Ошибка связи с 1PPS от антенны",                                    //
    "Ошибка АЦП (Мезонин)",                                              //
    "Ошибка АЦП (базовая)",                                              //
    "Произошла запись регулировочных коэффициентов (базовая)",           //
    "Произошла запись регулировочных коэффициентов (Мезонин)",           //
    "Произошло стирание системного журнала",                             //
    "Произошло стирание рабочего журнала",                               //
    "Произошло стирание осциллограмм",                                   //
    "Произошло стирание журнала измерений",                              //
    "Ошибка ВПО", "Ошибка встроенного АЦП",                              //
    "Произошла запись в блок Bhb (hidden block)",                        //
    "Произошла запись в блок Bhbm (hidden block)( Мезонин)"              //
};
const QStringList eventJourHeaders {
    " № ",              //
    "Дата/Время UTC",   //
    "Описание события", //
    "Тип события"       //
};
constexpr quint32 sysJourId = 0;
struct EventStruct
{
    quint64 Time;
    quint8 EvType;
    quint8 EvNum[3];
    quint32 Reserv;
};
const QStringList Health

    {
        "ADCI", //
        "FLS",  //
        "TUP",  //
        "ADCB", //
        "1PPS", //
        "ADCM", //
        "REGB", //
        "RCN",  //
        "HWIB", //
        "HWIM", //
        "REGM", //
        "BAT",  //
        "NTP",  //
        "FLS2", //
        "FRM"   //
    };

const QStringList HthToolTip {
    "Проблемы со встроенным АЦП ",                                                      //
    "Не работает внешняя flash-память",                                                 //
    "Перегрев",                                                                         //
    "Проблемы с АЦП (нет связи) (базовая)",                                             //
    "Нет сигнала 1PPS с антенны",                                                       //
    "Проблемы с АЦП (нет связи) (мезонин)",                                             //
    "Ошибка регулировочных коэффициентов (базовая)",                                    //
    "Ошибка загрузки конфигурации из flash-памяти. Работает конфигурация по умолчанию", //
    "Некорректная Hardware информация (базовая)",                                       //
    "Некорректная Hardware информация (мезонин)",                                       //
    "Ошибка регулировочных коэффициентов (мезонин)",                                    //
    "Напряжение батареи низко (< 2,5 В)",                                               //
    "Нет связи с NTP-сервером",                                                         //
    "Не работает внешняя flash-память (мезонин)",                                       //
    "Не работает внешняя fram",                                                         //
    "Проблемы со встроенным АЦП "                                                       //
};

}

namespace AVM_KTF
{
const QStringList criticalEvents {
    "Аварийное сообщение по недопустимому превышению температуры обмотки", //
    "Аварийное сообщение по недопустимому уровню пускового тока         "  //
};
const quint32 criticalCount = criticalEvents.size();
constexpr quint32 criticalFlags = 0x00000005;
const QStringList warningEvents {
    "Отсутствует сигнал напряжения фазы A                   ", //
    "Отсутствует сигнал напряжения фазы B                   ", //
    "Отсутствует сигнал напряжения фазы С                   ", //
    "Отсутствует сигнал тока  фазы А                        ", //
    "Отсутствует сигнал тока  фазы B                        ", //
    "Отсутствует сигнал тока  фазы C                        ", //
    "Перегрузка по току фазы A                              ", //
    "Перегрузка по току фазы В                              ", //
    "Перегрузка по току фазы С                              ", //
    //"Машина включена ",//
    "Сигнализация по опасному превышению температуры обмотки", //
    "Неисправны все датчики температуры обмотки             ", //
    "Сигнализация по опасному уровню пускового тока         "  //
};
constexpr quint32 warningFlags = 0xfffa;
constexpr quint32 workJourId = 5011;
struct MeasureStruct
{
    quint32 NUM;
    quint32 Time;
    float Ueff[3];
    float Ieff[3];
    float Frequency;
    float U0;
    float U1;
    float U2;
    float I0;
    float I1;
    float I2;
    float Pf[4];
    float Qf[4];
    float Sf[4];
    float Cosphi;
    float Twin;
    float Tmk;
    float Tamb;
    quint32 res;
};

const QStringList workJourDescription {
    "Отсутствует сигнал напряжения фазы A",                                //
    "Отсутствует сигнал напряжения фазы B",                                //
    "Отсутствует сигнал напряжения фазы С",                                //
    "Отсутствует сигнал тока фазы A",                                      //
    "Отсутствует сигнал тока фазы B",                                      //
    "Отсутствует сигнал тока фазы С",                                      //
    "Перегрузка по току фазы A",                                           //
    "Перегрузка по току фазы B",                                           //
    "Перегрузка по току фазы C",                                           //
    "Машина включена",                                                     //
    "Сигнализация по опасному превышению температуры обмотки",             //
    "Неисправны все датчики температуры об-мотки",                         //
    "Аварийное сообщение по недопустимому превышению температуры обмотки", //
    "Сигнализация по опасному уровню пускового тока",                      //
    "Аварийное сообщение по недопустимому уровню пускового тока"           //
};
const QStringList measJourHeaders {
    " № ",            //
    "Дата/Время UTC", //
    "Ueff фA",        //
    "Ueff фB",        //
    "Ueff фC",        //
    "Ieff фA",        //
    "Ieff фB",        //
    "Ieff фC",        //
    "Frequency",      //
    "U0",             //
    "U1",             //
    "U2",             //
    "I0",             //
    "I1",             //
    "I2",             //
    "Pf фA",          //
    "Pf фB",          //
    "Pf фC",          //
    "Pf сумм.",       //
    "Qf фA",          //
    "Qf фB",          //
    "Qf фC",          //
    "Qf сумм.",       //
    "Sf фA",          //
    "Sf фB",          //
    "Sf фC",          //
    "Sf сумм.",       //
    "CosPhi",         //
    "Tmk,°С",         //
    "Tamb,°С",        //
    "Twin,°С"         //
};
}

namespace AVM_KDV
{
const QStringList criticalEvents {
    "Аварийное сообщение по недопустимому превышению температуры обмотки", //
    "Аварийное сообщение по недопустимому уровню пускового тока         "  //
};
const quint32 criticalCount = criticalEvents.size();
constexpr quint32 criticalFlags = 0x00000005;
const QStringList warningEvents {
    "Отсутствует сигнал напряжения фазы A                   ", //
    "Отсутствует сигнал напряжения фазы B                   ", //
    "Отсутствует сигнал напряжения фазы С                   ", //
    "Отсутствует ток ввода фазы А (ток меньше 2мА)          ", //
    "Отсутствует ток ввода фазы B (ток меньше 2мА)          ", //
    "Отсутствует ток ввода фазы C (ток меньше 2мА)          ", //
    "Не заданы начальные значения                           ", //
    "Низкое напряжение фазы A                               ", //
    "Низкое напряжение фазы B                               ", //
    "Низкое напряжение фазы C                               ", //
    "Сигнализация по приращению тангенса дельта ввода фазы А", //
    "Сигнализация по приращению тангенса дельта ввода фазы B", //
    "Сигнализация по приращению тангенса дельта ввода фазы C", //
    "Сигнализация по приращению C ввода фазы А              ", //
    "Сигнализация по приращению C ввода фазы B              ", //
    "Сигнализация по приращению C ввода фазы C              ", //
    "Не заданы паспортные значения                          ", //
    "Сигнализация по повышенному небалансу токов            "  //
};
constexpr quint32 warningFlags = 0x1fff1ec6;
constexpr quint32 workJourId = 3010;
struct MeasureStruct
{
    quint32 NUM;
    quint32 Time;
    float Ueff[3];
    float Ieff[3];
    float Frequency;
    float U1;
    float U2;
    float I1;
    float I2;
    float Pf_sum;
    float Qf_sum;
    float Sf_sum;
    float CosPhi;
    float Tmk;
    float Tamb;
    float Twin;
    float VibrSCZ_V[6];
    float VibrSCZ_D[6];
};

const QStringList workJourDescription {
    "Отсутствует сигнал напряжения фазы A",                    //
    "Отсутствует сигнал напряжения фазы B",                    //
    "Отсутствует сигнал напряжения фазы С",                    //
    "Нет реактивного тока канала А (ток меньше 2мА)",          //
    "Нет реактивного тока канала B (ток меньше 2мА)",          //
    "Нет реактивного тока канала C (ток меньше 2мА)",          //
    "Не заданы начальные значения",                            //
    "Низкое напряжение фазы A",                                //
    "Низкое напряжение фазы B",                                //
    "Низкое напряжение фазы C",                                //
    "Сигнализация по приращению тангенса дельта ввода фазы А", //
    "Сигнализация по приращению тангенса дельта ввода фазы B", //
    "Сигнализация по приращению тангенса дельта ввода фазы C", //
    "Авария по приращению тангенса дельта ввода фазы А",       //
    "Авария по приращению тангенса дельта ввода фазы B",       //
    "Авария по приращению тангенса дельта ввода фазы C",       //
    "Сигнализация по приращению C ввода фазы А",               //
    "Сигнализация по приращению C ввода фазы B",               //
    "Сигнализация по приращению C ввода фазы C",               //
    "Авария по приращению C ввода фазы А",                     //
    "Авария по приращению C ввода фазы B",                     //
    "Авария по приращению C ввода фазы C",                     //
    "Не заданы паспортные значения",                           //
    "Сигнализация по изменению небаланса тока",                //
    "Авария по изменению небаланса тока"                       //
};
const QStringList measJourHeaders {
    " № ",            //
    "Дата/Время UTC", //
    "Ueff фA",        //
    "Ueff фB",        //
    "Ueff фC",        //
    "Ieff фA",        //
    "Ieff фB",        //
    "Ieff фC",        //
    "Freq",           //
    "U1",             //
    "U2",             //
    "I1",             //
    "I2",             //
    "Pf_sum",         //
    "Qf_sum",         //
    "Sf_sum",         //
    "CosPhi",         //
    "Tmk",            //
    "Tamb",           //
    "Twin",           //
    "VibrSCZ_V",      //
    "VibrSCZ_V",      //
    "VibrSCZ_V",      //
    "VibrSCZ_V",      //
    "VibrSCZ_V",      //
    "VibrSCZ_V",      //
    "VibrSCZ_D",      //
    "VibrSCZ_D",      //
    "VibrSCZ_D",      //
    "VibrSCZ_D",      //
    "VibrSCZ_D",      //
    "VibrSCZ_D"       //
};
}
