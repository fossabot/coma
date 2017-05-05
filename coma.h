#ifndef COMA_H
#define COMA_H

#include <QMainWindow>
#include <QResizeEvent>
#include "dialogs/fwupdialog.h"
#include "dialogs/downloaddialog.h"
#include "dialogs/oscdialog.h"
#include "dialogs/confdialog.h"
#include "dialogs/confdialog_21.h"
#include "dialogs/confdialog_80.h"
#include "check/checkdialog_21.h"
#include "check/checkdialog_80.h"
#include "tune/tunedialog_21.h"
#include "tune/tunedialog_80.h"
#include "canal.h"
#include "publicclass.h"

#define PROGNAME    "КОМА 2.0"

#define C_TE_MAXSIZE    10000
#define MAXERRORFLAGNUM 32

class Coma : public QMainWindow
{
    Q_OBJECT

public:
    Coma(QWidget *parent = 0);
    ~Coma();

signals:
    void mipparset();

private:
    bool SWHide;
    QRect SWGeometry;
    quint32 PrbSize;

    static QStringList Hth()
    {
        QStringList sl;
        sl.append("!OK");
        sl.append("FLS");
        sl.append("TUP");
        sl.append("REL");
        sl.append("1PPS");
        sl.append("ADC");
        sl.append("REG");
        sl.append("CNF");
        sl.append("LS");
        sl.append("FNC");
        sl.append("FW");
        sl.append("BAT");
        int ts = sl.size();
        for (int i=ts; i<MAXERRORFLAGNUM; ++i)
            sl.append("");
        return sl;
    }

    static QStringList HthToolTip()
    {
        QStringList sl;
        sl.append("Что-то не в порядке");
        sl.append("Ошибка внешней FLASH-памяти");
        sl.append("Перегрев модуля");
        sl.append("Неисправность выходных реле");
        sl.append("Нет сигнала 1PPS с антенны");
        sl.append("Нет связи с АЦП");
        sl.append("Нет настроечных параметров в памяти модуля");
        sl.append("Ошибка конфигурации в памяти модуля, взята конфигурация по умолчанию");
        sl.append("Сигналы слишком малы");
        sl.append("Частота находится вне диапазона");
        sl.append("Ошибка файла фирменного ПО");
        sl.append("Требуется замена батарейки");
        int ts = sl.size();
        for (int i=ts; i<MAXERRORFLAGNUM; ++i)
            sl.append("");
        return sl;
    }

    a_checkdialog *ACheckDialog;
    a_tunedialog *ATuneDialog;
    e_checkdialog *ECheckDialog;
    confdialog_80 *EConfDialog;
    e_tunedialog *ETuneDialog;
    fwupdialog *FwUpDialog;
    downloaddialog *DownDialog;
    oscdialog *OscDialog;
    QAction *WriteSNAction;
    quint8 ReconTry;
    publicclass::Bsi Bsi_block;
    QVector<publicclass::DataRec> S2Config;

    QString ByteToHex(quint8);
    void FillBsi();
    void ClearBsi();
    void ShowOrHideSlideSW();

signals:
    void CloseConnectDialog();

public slots:

private slots:
    void Stage2();
    void Exit();
    void Disconnect();
    void Stage1_5();
    void GetAbout();
    void UpdateMainTE(QByteArray ba);
    void SetPort(QString str);
    void OpenBhbDialog();
    void SendBhb();
    void MouseMove();
    void EmulA();
    void EmulE();
    void StartEmulE();
    void StartSettingsDialog();
    void ShowErrorDialog();
    void SetProgressBarSize(quint32);
    void SetProgressBar(quint32);
    void DisableProgressBar();

private:
    void SetupUI();
    void SetupMenubar();
    void PrepareTimers();
    void LoadSettings();
    void SaveSettings();
    void CopyBhbFromBsi();
    void Stage1();
    void Stage3();

protected:
    void resizeEvent(QResizeEvent *e);
};

#endif // COMA_H
