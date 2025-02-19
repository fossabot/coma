#ifndef IEC104THREAD_H
#define IEC104THREAD_H

#include "../gen/datatypes.h"
#include "../gen/error.h"
#include "../gen/logclass.h"

#include <QQueue>
#include <QTimer>
#include <QVariant>
class IEC104Thread : public QObject
{
    Q_OBJECT
public:
    /*    typedef struct
        {
            FLOAT104 fl;
            int SigNumber;
        } FlSignals104;

        typedef struct
        {
            SIQ104withTime Spon[256];
            int SigNumber;
        } SponSignals;

        typedef struct
        {
            BS104 BS;
            int SigNumber;
        } BS104Signals; */

    IEC104Thread(LogClass *log, QObject *parent = nullptr);
    ~IEC104Thread();

    void SetBaseAdr(quint16 adr);

public slots:
    void StartDT();
    void StopDT();
    void Stop();
    void Run();
    void GetSomeData(QByteArray);

signals:
    //  void Started();
    void finished();
    void WriteData(QByteArray);
    void ReconnectSignal();
    //    void Floatsignalsreceived(IEC104Thread::FlSignals104 *);
    //    void Sponsignalsreceived(IEC104Thread::SponSignals *);
    //    void Bs104signalsreceived(IEC104Thread::BS104Signals *);
    //    void SendS2fromParse(S2ConfigType *);
    //    void SendJourSysfromParse(QByteArray);
    //    void SendJourWorkfromParse(QByteArray);
    //    void SendJourMeasfromParse(QByteArray);
    //    void SendMessageOk();
    //    void SetDataSize(int);
    //    void SetDataCount(int);
    void SendMessagefromParse();

private:
    typedef QByteArray APCI, ASDU;

    static QMutex s_ParseReadMutex;
    static QMutex s_ParseWriteMutex;
    //    int incLS;
    int m_signalCounter, m_noAnswer;
    bool m_isFileSending;
    bool m_fileIsConfigFile; // flag indicates that S2::RestoreData should be executed upon file received
    QByteArray m_file;
    QList<QByteArray> m_parseData;
    //    quint32 ReadDataSize;
    quint16 m_V_S, m_V_R, m_ackVR;
    int m_command;
    //    S2ConfigType *DR; // ссылка на структуру DataRec, по которой собирать/восстанавливать S2
    //    S2ConfigType *DRJour;
    quint32 m_fileLen;
    QTimer *m_timer104;
    QByteArray m_readData;
    quint8 m_readSize; // длина всей посылки
    int m_readPos;
    bool m_threadMustBeFinished;
    quint8 m_APDULength;
    quint8 m_APDUFormat;
    quint8 m_sectionNum;
    LogClass *m_log;
    quint8 m_baseAdrHigh, m_baseAdrLow;
    QTimer *m_sendTestTimer;
    quint8 m_KSS;
    quint8 m_KSF;
    QByteArray m_cutPckt;
    bool m_isFirstParse;
    bool m_writingToPortBlocked;

    void ParseIFormat(QByteArray &ba);
    Error::Msg isIncomeDataValid(QByteArray);
    QByteArray CreateGI(unsigned char apdulength);
    QByteArray ASDUFilePrefix(unsigned char Command, unsigned char filenum, unsigned char secnum);
    QByteArray ASDU6Prefix(unsigned char Command, quint32 adr);
    template <typename T> QByteArray ToByteArray(T var);
    void Send(int inc, APCI, ASDU = QByteArray());
    void SendGI();
    void SendS();
    void SendTestCon();
    //    void CorReadRequest();
    void SelectFile(char numfile);
    void CallFile(unsigned char);
    void GetSection(unsigned char);
    void ConfirmSection(unsigned char);
    void ConfirmFile(unsigned char);
    void FileReady(quint16 numfile);
    void SectionReady();
    void SendSegments();
    void LastSection();
    void Com45(quint32 com);
    void Com50(quint32 adr, float param);
    void reqGroup(int groupNum);
    void Com51WriteTime(quint32 time);
    //    void convert(IEC104Thread::SponSignals *signal);
    void setGeneralResponse(DataTypes::GeneralResponseTypes type, quint64 data = 0);
    bool handleFile(QByteArray &ba, DataTypes::FilesEnum addr, bool isConfigFile);
private slots:
    void SendTestAct();

protected:
};

#endif // IEC104THREAD_H
