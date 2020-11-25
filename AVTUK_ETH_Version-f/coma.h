#ifndef COMA_H
#define COMA_H

#include "../interfaces/settingstypes.h"
#include "../module/module.h"

#include <QMainWindow>

enum INTERVAL
{
    RECONNECT = 3000,
    WAIT = 30000
};

enum THREAD
{
    USB = 0x01,
    P104 = 0x02,
    MODBUS = 0x04
};

class Coma : public QMainWindow
{
    Q_OBJECT

public:
    enum Modes
    {
        COMA_GENERALMODE, // обычный режим
        COMA_AUTON_OSCMODE, // автономный режим с отображением сохранённой осциллограммы
        COMA_AUTON_PROTMODE, // автономный режим с отображением протокола из прибора
        COMA_AUTON_SWJMODE, // автономный режим с отображением сохранённого журнала
        COMA_AUTON_MODE // просто автономный режим
    };

    typedef struct
    {
        quint32 SigVal;
        quint8 SigQuality;
        // quint64 CP56Time;
    } BS104;

    struct Bd11
    {
        quint32 dev;
        quint32 predAlarm;
        quint32 alarm;
    };

    Coma(QWidget *parent = nullptr);
    ~Coma();
    void SetMode(int mode);
    void Go(const QString &parameter = "");
    void ClearTW();
    void SetupMenubar();
    //    QWidget *MainInfoWidget();

    QWidget *Least();
    // Error::Msg CheckPassword();
    void Disconnect();
    void Connect();

signals:
    void CloseConnectDialog();
    // void PasswordChecked();
    //    void ClearBsi();
    void Finished();
    void StopCommunications();
    // void ConnectMes(QString *);
    // void stateChanged(bool);

public slots:
    void DisconnectAndClear();

    void FileTimeOut();
    void ReConnect();
    void AttemptToRec();
    void ConnectMessage();

private slots:
    void StartWork();
    void GetAbout();
    void closeEvent(QCloseEvent *event) override;
    //    void SetDefConf();
    void update(const DataTypes::GeneralResponseStruct &rsp);

    //    void setConf(unsigned char);
    //    void Fill();
    //    void FillBSI(IEC104Thread::BS104Signals *sig);
    //    void FillBSI(QList<ModBus::BSISignalStruct> sig, unsigned int sigsize);
    // void PasswordCheck(QString psw);
    void MainTWTabClicked(int tabindex);

private:
    // constexpr QVector<int> MTBs = { 0x21, 0x22, 0x31, 0x35, 0x80, 0x81, 0x84 };

    Module *m_Module;
    AlarmWidget *AlarmW;
    //    WarnKIV *WarnKIVDialog;
    //    AlarmKIV *AlarmKIVDialog;
    //    WarnKTF *WarnKTFDialog;
    //    AlarmKTF *AlarmKTFDialog;
    //    AlarmStateAll *AlarmStateAllDialog;
    //    AlarmClass *Alarm;
    //    QWidget *Parent;

    //    InfoDialog *infoDialog;
    //    ConfDialog *mainConfDialog;
    //    ConfDialog *mainTuneDialog;
    //    AbstractStartupDialog *corDialog;
    //    AbstractConfDialog *confBDialog, *confMDialog;
    //    AbstractCheckDialog *checkBDialog, *checkMDialog, *HarmDialog, *VibrDialog;
    //    JournalDialog *jourDialog;
    //    FWUploadDialog *fwUpDialog;
    //    TimeDialog *timeDialog;

    //    bool PasswordValid;
    bool TimeThrFinished;
    bool Cancelled;
    bool Reconnect;
    int Mode; // режим запуска программы
    int fileSize, curfileSize;
    //    int CheckIndex, TimeIndex, ConfIndex, CheckHarmIndex, CheckVibrIndex, CurTabIndex;
    quint8 HaveAlreadyRed = 0;
    //    quint8 ActiveThreads;
    bool ActiveThreads;
    quint32 Mes;

    QString SavePort;

    //    QVector<S2::DataRec> *S2ConfigForTune;
    //    QVector<S2::DataRec> *S2Config;
    S2DataTypes::S2ConfigType *S2Config;

    //    quint8 PredAlarmEvents[20];
    //    quint8 AlarmEvents[20];

    QTimer *ReceiveTimer, *m_BSITimer;
    //    QTimer *ReconnectTimer;
    QTimer *BdaTimer, *AlrmTimer; //, *HarmTimer, *VibrTimer;

    //    IEC104 *Ch104;
    //    ModBus *ChModbus;
    BaseInterface *m_iface;

    ConnectStruct ConnectSettings;

    void LoadSettings();
    void SaveSettings();
    void SetProgressBarSize(int prbnum, int size);
    void SetProgressBar(int prbnum, int cursize);
    void New104();
    void NewModbus();
    void NewUSB();
    void newTimers();
    void SetupUI();
    void CloseDialogs();
    void PrepareDialogs();
    void setupConnections();
    void SetProgressBar1Size(int size);
    void SetProgressBar1(int cursize);
    //    void SetProgressBar2Size(int size);
    //    void SetProgressBar2(int cursize);

    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

    //    void addConfTab(ETabWidget *MainTW, QString str);
    // Дурацкое название
    //    void setupQConnections();

    //    void setupDialogs(ETabWidget *MainTW);

    QToolBar *createToolBar();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
};

#endif // COMA_H
