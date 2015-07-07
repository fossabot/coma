#include <QThread>
#include <QTime>
#include <QCoreApplication>

#include "iec104.h"
#include "ethernet.h"
#include "../publicclass.h"

iec104::iec104(QObject *parent) : QObject(parent)
{
    GSD = true;
    QThread *thr = new QThread;
    ethernet *eth = new ethernet;
    eth->moveToThread(thr);
    connect(thr, SIGNAL(started()), eth, SLOT(run()));
    connect(eth,SIGNAL(finished()),thr,SLOT(quit()));
    connect(this,SIGNAL(stopall()),eth,SLOT(stop()));
    connect(eth,SIGNAL(finished()),eth,SLOT(deleteLater()));
    connect(thr,SIGNAL(finished()),thr,SLOT(deleteLater()));
    connect(eth,SIGNAL(connected()),this,SIGNAL(ethconnected()));
    connect(eth,SIGNAL(connected()),this,SLOT(Start()));
    connect(eth,SIGNAL(error(int)),this,SIGNAL(error(int)));
    connect(eth,SIGNAL(newdataarrived(QByteArray)),this,SLOT(GetSomeData(QByteArray)));
    connect(this,SIGNAL(writedatatoeth(QByteArray)),eth,SLOT(InitiateWriteDataToPort(QByteArray)));

    QThread *ParseThread = new QThread;
    Parse = new Parse104;
    Parse->moveToThread(ParseThread);
    connect(ParseThread,SIGNAL(started()),Parse,SLOT(ParseIncomeData()));
    connect(Parse,SIGNAL(finished()),ParseThread,SLOT(quit()));
    connect(this,SIGNAL(stopall()),Parse,SLOT(stop()));
    connect(Parse,SIGNAL(finished()),Parse,SLOT(deleteLater()));
    connect(ParseThread,SIGNAL(finished()),ParseThread,SLOT(deleteLater()));

    connect(Parse,SIGNAL(signalsreceived()),this,SLOT(SignalsGot()));
    connect(Parse,SIGNAL(sendS()),this,SLOT(SendS()));
    ParseThread->start();
    thr->start();
}

iec104::~iec104()
{
}

void iec104::Start()
{
    APCI StartDT;
    StartDT.start = I104_START;
    StartDT.APDUlength = 4;
    StartDT.contrfield[0] = I104_STARTDT_ACT;
    StartDT.contrfield[1] = StartDT.contrfield[2] = StartDT.contrfield[3] = 0;
    Parse->cmd = I104_STARTDT_ACT;
    Send(StartDT); // ASDU = QByteArray()
}

void iec104::Send(APCI apci, ASDU asdu)
{
    QByteArray ba;
    ba.clear();
    void *tempp = &apci;
    ba.append(static_cast<char *>(tempp),sizeof(apci));
    if (!asdu.isEmpty())
    {
        tempp = &asdu;
        ba.append(static_cast<char *>(tempp),sizeof(asdu));
    }
    emit writedatatoeth(ba);
}

void iec104::GetSomeData(QByteArray ba)
{
    ParseSomeData(ba, true);
    Parse->NewDataArrived = true;
}

void iec104::ParseSomeData(QByteArray ba, bool GSD)
{
    quint32 basize = static_cast<quint32>(ba.size());
    if (GSD)
    {
        if (cutpckt.size()>0)
        {
            int missing_num = cutpckt.at(1)+2-cutpckt.size(); // взяли длину остатка от предыдущего пакета
            QByteArray missing_arr = ba.left(missing_num); // взяли из текущего пакета сами байты
            ba = ba.right(basize-missing_num);
            cutpckt.append(missing_arr);
            Parse->ParseData.append(cutpckt);
            cutpckt.clear();
        }
    }
    quint8 BlockLength = static_cast<quint8>(ba.at(1))+2;
    if (basize == BlockLength)
        Parse->ParseData.append(ba);
    else if (basize < BlockLength)
        cutpckt = ba;
    else
    {
        Parse->ParseData.append(ba.left(BlockLength));
        ParseSomeData(ba.right(basize-BlockLength), false);
    }
}

void iec104::SignalsGot()
{
    Parse->SignalsMutex.lock();
    Signals=Parse->Signals;
/*    Signals.SigNum=Parse->Signals.SigNum;
    Signals.SigQuality=Parse->Signals.SigQuality;
    Signals.SigVal=Parse->Signals.SigVal;
    Signals.CP56Time=Parse->Signals.CP56Time;
    Parse->Signals.SigNum.clear();
    Parse->Signals.SigVal.clear();
    Parse->Signals.SigQuality.clear();
    Parse->Signals.CP56Time.clear(); */
    Parse->SignalsMutex.unlock();
    emit signalsready();
}

void iec104::SendS()
{
    APCI Confirm;
    Confirm.start = I104_START;
    Confirm.APDUlength = 4;
    Confirm.contrfield[0] = I104_S;
    Confirm.contrfield[1] = 0x00;
    quint16 VR = Parse->V_R;
    Confirm.contrfield[2] = (VR & 0x007F) << 1;
    Confirm.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;
    Send(Confirm); // ASDU = QByteArray()
}

// Класс PARSE104

Parse104::Parse104(QObject *parent) : QObject(parent)
{
    ParseData.clear();
    ThreadMustBeFinished = false;
    V_S = V_R = 0;
    AckVR = I104_W;
    APDUFormat = I104_WRONG;
    ReceiverBusy = false;
    GetNewVR = false;
    NewDataArrived = false;
}

Parse104::~Parse104()
{
}

void Parse104::ParseIncomeData()
{
    while (!ThreadMustBeFinished)
    {
        if (NewDataArrived)
        {
            NewDataArrived = false;
            // обработка ParseData
            while (ParseData.size())
            {
                if (ParseData.at(0).isEmpty())
                {
                    ParseData.removeFirst();
                    continue;
                }
                char *ParseDataOne = new char(ParseData.at(0).size());
                memcpy(ParseDataOne,ParseData.at(0).constData(),ParseData.at(0).size());
                ParseData.removeFirst();
                if (ParseDataOne[0] != 0x68) // первый байт не 0x68 - кривая посылка
                    continue;
                else
                {
                    int tmpi = isIncomeDataValid(ParseDataOne);
                    if (tmpi == I104_RCVNORM) // если поймали правильное начало данных, переходим к их обработке
                    {
                        if (APDUFormat == I104_I)
                            ParseIFormat(&(ParseDataOne[6])); // без APCI
                    }
                }
                if (V_R>AckVR)
                {
                    emit sendS();
                    GetNewVR = true;
                }
            }
        }
        else
        {
            QTime tmr;
            tmr.start();
            while (tmr.elapsed() < 100)
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
    }
    emit finished();
}

int Parse104::isIncomeDataValid(char *ba)
{
    APDULength = ba[1]; // в 1-м байте лежит длина
    if ((APDULength<4) || (APDULength>253))
    {
        emit error(M104_LENGTHER);
        return I104_RCVWRONG;
    }
    if (!(ba[2]&0x01)) // I
        APDUFormat = I104_I;
    else
    {
        if (!(ba[2]&0x02)) // S
            APDUFormat = I104_S;
        else
            APDUFormat = I104_U;
    }
    switch (APDUFormat)
    {
    case I104_I:
    {
        quint16 V_Rrcv = static_cast<quint16>(ba[3])*256+static_cast<quint16>(ba[2]&0xFE);
        V_Rrcv >>= 1;
        if (GetNewVR)
        {
            V_R = V_Rrcv;
            GetNewVR = false;
            AckVR = V_R + I104_W;
        }
        if (V_Rrcv != V_R)
        {
            emit error(M104_NUMER);
            return I104_RCVWRONG;
        }
        quint16 V_Srcv = static_cast<quint16>(ba[5])*256+static_cast<quint16>(ba[4]&0xFE);
        V_Srcv >>= 1;
        if (V_Srcv != V_S)
        {
            emit error(M104_NUMER);
            return I104_RCVWRONG;
        }
        V_R++;
        return I104_RCVNORM;
        break;
    }
    case I104_S:
    {
        quint16 V_Srcv = static_cast<quint16>(ba[5])*256+static_cast<quint16>(ba[4]&0xFE);
        V_Srcv >>= 1;
        if (V_Srcv != V_S)
        {
            emit error(M104_NUMER);
            return I104_RCVWRONG;
        }
//        V_R++;
        return I104_RCVNORM;
        break;
    }
    case I104_U:
    {
        if ((ba[2] == I104_STARTDT_CON) && (cmd == I104_STARTDT_ACT)) // если пришло подтверждение старта и перед этим мы старт запрашивали
            cmd = I104_STARTDT_CON;
        if ((ba[2] == I104_STOPDT_CON) && (cmd == I104_STOPDT_ACT)) // если пришло подтверждение стопа и перед этим мы стоп запрашивали
            cmd = I104_STOPDT_CON;
        if ((ba[2] == I104_TESTFR_CON) && (cmd == I104_TESTFR_ACT)) // если пришло подтверждение теста и перед этим мы тест запрашивали
            cmd = I104_TESTFR_CON;
        return I104_RCVNORM;
        break;
    }
    default:
        break;
    }
    return I104_RCVWRONG;
}

void Parse104::ParseIFormat(char *ba) // основной разборщик
{
    SignalsMutex.lock();
    DUI.typeIdent = ba[0];
    DUI.qualifier.Number = ba[1]&0x7f;
    DUI.qualifier.SQ = ba[1]>>7;
    DUI.cause.cause = ba[2]&0x3F;
    DUI.cause.confirm = (ba[2]>>6)&0x01;
    DUI.cause.test = ba[2]>>7;
    DUI.cause.initiator = ba[3];
    DUI.commonAdrASDU = ba[4] + ba[5]*256;
    quint32 ObjectAdr;
    quint32 index = 6;
    int i;
    for (i = 0; i < DUI.qualifier.Number; i++)
    {
        if ((i==0) || (DUI.qualifier.SQ==0))
        {
            ObjectAdr = ba[index++];
            ObjectAdr += ba[index++]*0x100;
            ObjectAdr += ba[index++]*0x10000;
        }
        else
            ObjectAdr++;
        switch(DUI.typeIdent)
        {
        case M_EI_NA_1: // 70 тип - подтверждение окончания инициализации
        {
            break;
        }
        case M_ME_TF_1: // 36 тип - измеренные данные с плавающей запятой
        {
            if (ObjectAdr > 17) // для регулировки Э значения сигналов МИПа с индексом более 17 не требуются
            {
                index += 12;
                break;
            }
            Signals.SigNum.append(ObjectAdr);
            float value;
            memcpy(&value,&ba[index],4);
            index += 4;
            Signals.SigVal.append(QString::number(value,'g',6));
            quint8 quality;
            memcpy(&quality,&ba[index],1);
            index++;
            Signals.SigQuality.append(quality);
            quint64 time;
            memcpy(&time,&ba[index],7);
            index += 7;
            Signals.CP56Time.append(time);
            break;
        }
        default:
            break;
        }
    }
    SignalsMutex.unlock();
    emit signalsreceived();
}

void Parse104::stop()
{
    ThreadMustBeFinished = true;
}
