#include "modbusthread.h"

#include "../gen/datamanager.h"
#include "../gen/helper.h"
#include "../gen/pch.h"
#include "../gen/stdfunc.h"
#include "baseinterface.h"

#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QTimer>
#include <QtEndian>
#include <algorithm>
//#define MODBUS_DEBUG
ModbusThread::ModbusThread(QObject *parent) : QObject(parent), Log(new LogClass(this))
{
    Log->Init("modbus.log");
    Log->info("=== Log started ===");
    // AboutToFinish = false;
}

ModbusThread::~ModbusThread()
{
}

void ModbusThread::setDeviceAddress(quint8 adr)
{
    deviceAddress = adr;
}

void ModbusThread::Run()
{
    const auto &iface = BaseInterface::iface();
    using State = BaseInterface::State;
    while (iface->state() != State::Stop)
    //  while (!AboutToFinish)
    {
        CommandsMBS::CommandStruct inp;
        if (DataManager::deQueue(inp) == Error::Msg::NoError)
        {
#ifdef MODBUS_DEBUG
            qDebug() << inp.cmd << inp.adr << inp.quantity;
#endif
            switch (inp.cmd)
            {
            case CommandsMBS::MBS_READINPUTREGISTER:
                readRegisters(inp); // send command to the port
                break;
                //                Com45(inp.uintarg);
                //                RunMutex.lock();
                //                RunWC.wait(&RunMutex, 20);
                //                RunMutex.unlock();
                //        while (!InQueue->isEmpty())
                //        {
                //            InMutex.lock();
                //            //            ModBus::InOutStruct inp = InQueue->dequeue();
                //            Queries::CommandMBS inp;
                //            if (DataManager::deQueue(inp) == Error::Msg::NoError)
                //                InMutex.unlock();
                //            SendAndGetResult(inp);
            case CommandsMBS::MBS_READHOLDINGREGISTERS:
                readRegisters(inp);
                break;
            case CommandsMBS::MBS_WRITEMULTIPLEREGISTERS:
                writeMultipleRegisters(inp);
                break;
            case CommandsMBS::MBS_READCOILS:
                readCoils(inp);
                break;
            default:
                break;
            }
        }
        QCoreApplication::processEvents();
    }
    emit finished();
}
// void ModbusThread::SendAndGetResult(ModBus::InOutStruct &inp)
//{
//    Inp = inp;
//    Send();
//    Outp.TaskNum = Inp.TaskNum;
//    AddToOutQueue(Outp);
//}

// void ModbusThread::AddToOutQueue(ModBus::InOutStruct &outp)
//{
//    OutMutex.lock();
//    OutList->append(outp);
//    OutMutex.unlock();
//    OutWC.wakeAll();
//}

// void ModbusThread::Send()
//{
//    // data to send is in Inp.Ba
//    quint16 KSS = CalcCRC(Inp.Ba);
//    Inp.Ba.append(static_cast<char>(KSS >> 8));
//    Inp.Ba.append(static_cast<char>(KSS));
//    Outp.Ba.clear();
//    Inp.Checked = false;
//    Busy = true;
//    Log->info("-> " + Inp.Ba.toHex());
//    //    qint64 st = SerialPort->write(Inp.Ba.data(), Inp.Ba.size());
//    emit Write(Inp.Ba);
//    /*    if (st < Inp.Ba.size())
//        {
//            Outp.Res = RESEMPTY;
//            return;
//        } */
//    QElapsedTimer tme;
//    tme.start();
//    while ((Busy) && (tme.elapsed() < RECONNECTTIME)) // ждём, пока либо сервер не отработает,
//        // либо не наступит таймаут
//        QCoreApplication::processEvents();
//    if (Busy)
//    {
//        Busy = false;
//        Log->error("Timeout");
//        Outp.Res = Error::Msg::GeneralError;
//    }
//}

void ModbusThread::ParseReply(QByteArray ba)
{

    m_readData.append(ba);
    if (m_readData.size() >= 2)
    {
        quint8 receivedCommand = m_readData.at(1);
        if ((receivedCommand & 0x80) || (receivedCommand != m_commandSent.cmd))
        {
            Log->error("Modbus error response: " + m_readData.toHex());
            qCritical() << Error::ReadError << metaObject()->className();
            m_readData.clear();
            Busy = false;
            return;
        }
    }
    if (m_readData.size() >= m_bytesToReceive)
    {
        Log->info("<- " + m_readData.toHex());
        int rdsize = m_readData.size();

        quint16 crcfinal = (static_cast<quint8>(m_readData.data()[rdsize - 2]) << 8)
            | (static_cast<quint8>(m_readData.data()[rdsize - 1]));
        m_readData.chop(2);
        quint16 MYKSS = CalcCRC(m_readData);

        if (MYKSS != crcfinal)
        {
            Log->error("Crc error");
            qCritical() << Error::CrcError << metaObject()->className();
            m_readData.clear();
        }
        // add to out list
        parseAndSetToOutList(m_readData);
        Busy = false;
        return;
    }
#ifdef MODBUS_DEBUG
    qDebug() << Error::SizeError << "Wait for:" << m_bytesToReceive << "Received: " << ba.size();
    qDebug() << m_commandSent /* << ba*/;
    QDebug deb = qDebug();
    for (const quint8 byte : ba)
        deb << QString::number(byte, 16);
#endif
    //    Outp.Ba.append(ba);
    //    //    Log->info("<- " + Outp.Ba.toHex());
    //    if ((!Inp.Checked) && (Outp.Ba.size() >= 2))
    //    {
    //        Inp.Checked = true;
    //        if (Outp.Ba.at(1) == (Inp.Command & 0x80)) // error
    //        {
    //            Log->error("Error in module");
    //            ERMSG("modbus error response: " + Outp.Ba.toHex());
    //        }
    //        if (Outp.Ba.at(1) != Inp.Command) // wrong response
    //        {
    //            Log->error("Wrong response: " + Outp.Ba.toHex());
    //            ERMSG("modbus wrong command");
    //        }
    //    }

    //    if (Outp.Ba.size() >= Inp.ReadSize)
    //    {
    //        Log->info("<- " + Outp.Ba.toHex());
    //        int rdsize = Outp.Ba.size();

    //        quint16 crcfinal = (static_cast<quint8>(Outp.Ba.data()[rdsize - 2]) << 8)
    //            | (static_cast<quint8>(Outp.Ba.data()[rdsize - 1]));
    //        Outp.Ba.chop(2);
    //        quint16 MYKSS = CalcCRC(Outp.Ba);

    //        if (MYKSS != crcfinal)
    //        {
    //            Log->error("Crc error");
    //            ERMSG("modbus crc error");
    //            //            emit ErrorCrc();
    //            Outp.Res = Error::Msg::GeneralError;
    //            return;
    //        }
    //        Outp.Res = Error::Msg::NoError;
    //        Busy = false;
    //    }
}

void ModbusThread::send(QByteArray &ba)
{
    quint16 KSS = CalcCRC(ba);
    ba.append(static_cast<char>(KSS >> 8));
    ba.append(static_cast<char>(KSS));
    sendWithoutCrc(ba);
}

void ModbusThread::sendWithoutCrc(const QByteArray &ba)
{
    m_readData.clear();
    Busy = true;
    Log->info("-> " + ba.toHex());
    emit Write(ba);
    QElapsedTimer tme;
    //    QTimer timer;
    //    timer.setSingleShot(true);

    //    connect(&timer, &QTimer::timeout, [=]() {
    //        if (Busy)
    //        {
    //            Busy = false;
    //            Log->error("Timeout");
    //        }
    //    });
    //    timer.start(RECONNECTTIME);
    tme.start();
    while ((Busy) && (tme.elapsed() < RECONNECTTIME)) // ждём, пока либо сервер не отработает,
        // либо не наступит таймаут
        QCoreApplication::processEvents();
    if (Busy)
    {
        Busy = false;
        emit clearBuffer();
        qWarning() << Error::Timeout;
        Log->error("Timeout");
    }
}

void ModbusThread::parseAndSetToOutList(QByteArray &ba)
{
    //    switch (m_commandSent.cmd)
    //    {
    //    case CommandsMBS::MBS_READINPUTREGISTER:
    //    {
    //        getFloatSignals(ba);
    //        break;
    //    }
    //    case CommandsMBS::MBS_READHOLDINGREGISTERS:
    //    {
    //        getIntegerSignals(ba);
    //        break;
    //    }
    //    case CommandsMBS::MBS_WRITEMULTIPLEREGISTERS:
    //    {
    //        getCommandResponse(ba);
    //        break;
    //    }
    //    case CommandsMBS::MBS_READCOILS:
    //    {
    //        getSinglePointSignals(ba);
    //        break;
    //    }
    //    default:
    //        break;
    //    }
    using namespace CommandsMBS;
    switch (m_commandSent.type)
    {
    case TypeId::Float:
        getFloatSignals(ba);
        break;
    case TypeId::Uint32:
        getIntegerSignals(ba);
        break;
    case TypeId::Bool:
        getSinglePointSignals(ba);
        break;
    default:
        getCommandResponse(ba);
        break;
    }
}

void ModbusThread::getFloatSignals(QByteArray &bain)
{
    if (bain.size() < 3)
    {
        Log->error("Wrong inbuf size");
        qCritical() << Error::SizeError << metaObject()->className();
        return;
    }
    int byteSize = bain.data()[2];
    QByteArray ba = bain.mid(3);
    if (byteSize > ba.size())
    {
        qCritical() << Error::SizeError << metaObject()->className();
        return;
    }
    for (auto i = 0; i < ba.size(); i += sizeof(float))
    {
        DataTypes::FloatStruct signal;
        signal.sigVal = unpackReg<float>(ba.mid(i, sizeof(float)));
        signal.sigAdr = m_commandSent.adr + i / sizeof(float);
        DataManager::addSignalToOutList(DataTypes::SignalTypes::Float, signal);
    }
}

void ModbusThread::getIntegerSignals(QByteArray &bain)
{
    if (bain.size() < 3)
    {
        Log->error("Wrong inbuf size");
        qCritical() << Error::SizeError << metaObject()->className();
        return;
    }
    int byteSize = bain.data()[2];
    QByteArray ba = bain.mid(3);
    if (byteSize > ba.size())
    {
        qCritical() << Error::SizeError << metaObject()->className();
        return;
    }
    for (auto i = 0; i < ba.size(); i += sizeof(quint32))
    {
        DataTypes::BitStringStruct signal;
        signal.sigVal = unpackReg<quint32>(ba.mid(i, sizeof(quint32)));
        signal.sigAdr = m_commandSent.adr + i / sizeof(quint32);
        DataManager::addSignalToOutList(DataTypes::SignalTypes::BitString, signal);
    }
}

void ModbusThread::getCommandResponse(QByteArray &bain)
{
    if (bain.size() < 3)
    {
        qCritical() << Error::SizeError << metaObject()->className();
        // Log->error("Wrong inbuf size");
        return;
    }
    // ?
    int byteSize = bain.at(2);
    QByteArray ba = bain.mid(3);
    if (byteSize > ba.size())
    {
        qCritical() << Error::SizeError << metaObject()->className();
        // qCritical("Wrong byte size in response");
        return;
    }
    DataTypes::GeneralResponseStruct grs;
    grs.type = DataTypes::GeneralResponseTypes::Ok;
    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, grs);
}

void ModbusThread::getSinglePointSignals(QByteArray &bain)
{
    DataTypes::SinglePointWithTimeStruct signal;

    if (bain.size() < 3)
    {
        qCritical() << Error::SizeError << metaObject()->className();
        //  Log->error("Wrong inbuf size");
        return;
    }
    int byteSize = bain.data()[2];
    QByteArray ba = bain.mid(3);
    if (byteSize > ba.size())
    {
        qCritical() << Error::SizeError << metaObject()->className();
        // ERMSG("Wrong byte size in response");
        return;
    }
    for (int i = 0; i < byteSize; ++i)
    {
        quint8 ival = ba.at(i);
        for (int j = 0; j < 8; ++j)
        {
            signal.sigAdr = m_commandSent.adr + i * 8 + j;
            signal.sigVal = ((0x01 << j) & ival) ? 1 : 0;
            DataManager::addSignalToOutList(DataTypes::SignalTypes::SinglePointWithTime, signal);
        }
    }
}

void ModbusThread::FinishThread()
{
    qDebug() << __PRETTY_FUNCTION__;
    // AboutToFinish = true;
}

quint16 ModbusThread::CalcCRC(QByteArray &ba) const
{
    quint8 CRChi = 0xFF;
    quint8 CRClo = 0xFF;
    quint8 Ind;
    quint16 crc;
    int count = 0;

    while (count < ba.size())
    {
        Ind = CRChi ^ ba.at(count);
        count++;
        CRChi = CRClo ^ TabCRChi[Ind];
        CRClo = TabCRClo[Ind];
    }
    crc = ((CRChi << 8) | CRClo);
    return crc;
}

void ModbusThread::readRegisters(CommandsMBS::CommandStruct &cms)
{

    m_commandSent = cms;
    QByteArray ba(createReadPDU(cms));
    ba = createADU(ba);
    if (cms.data.size())
        ba.append(cms.data);
    Log->info("Send bytes: " + ba.toHex());
    m_bytesToReceive = cms.quantity * 2 + 5; // address, function code, bytes count, crc (2)
    sendWithoutCrc(ba);
}

void ModbusThread::readCoils(CommandsMBS::CommandStruct &cms)
{
    m_commandSent = cms;
    QByteArray ba(createReadPDU(cms));
    ba = createADU(ba);
    Log->info("Send bytes: " + ba.toHex());
    auto temp = cms.quantity / 2 + cms.quantity % 2;
    temp = temp / 8 + temp % 8;  // try to count bytes for coils
    m_bytesToReceive = temp + 3; // address, function code, bytes count, crc (2)
    sendWithoutCrc(ba);
}

void ModbusThread::writeMultipleRegisters(CommandsMBS::CommandStruct &cms)
{
    QByteArray ba;
    setQueryStartBytes(cms, ba);
    ba.append(cms.quantity * 2);
    if (cms.data.size())
        ba.append(cms.data);
    Log->info("Send bytes: " + ba.toHex());
    m_bytesToReceive = 8; // address, function code, address (2), quantity (2), crc (2)
    send(ba);
}

void ModbusThread::setQueryStartBytes(CommandsMBS::CommandStruct &cms, QByteArray &ba)
{
    m_commandSent = cms;
    ba.append(deviceAddress); // адрес устройства
    ba.append(cms.cmd);       //аналоговый выход
    QByteArray bigEndArray;
    bigEndArray = StdFunc::arrayFromNumber(qToBigEndian(cms.adr));
    ba.append(bigEndArray);
    //  ba.append(static_cast<char>((cms.adr & 0xFF00) >> 8));
    //  ba.append(static_cast<char>(cms.adr & 0x00FF));
    bigEndArray = StdFunc::arrayFromNumber(qToBigEndian(cms.quantity));
    ba.append(bigEndArray);
    //  ba.append(static_cast<char>((cms.quantity & 0xFF00) >> 8));
    //   ba.append(static_cast<char>(cms.quantity & 0x00FF));
}

QByteArray ModbusThread::createReadPDU(const CommandsMBS::CommandStruct &cms) const
{
    QByteArray ba;
    ba.append(cms.cmd);
    ba.append(StdFunc::arrayFromNumber(qToBigEndian(cms.adr)));
    ba.append(StdFunc::arrayFromNumber(qToBigEndian(cms.quantity)));
    return ba;
}

QByteArray ModbusThread::createADU(const QByteArray &pdu) const
{
    QByteArray ba;
    ba.append(deviceAddress);
    ba.append(pdu);
    quint16 KSS = CalcCRC(ba);
    ba.append(static_cast<char>(KSS >> 8));
    ba.append(static_cast<char>(KSS));
    return ba;
}
