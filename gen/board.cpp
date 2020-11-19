#include "board.h"

#include "../module/modules.h"
#include "../module/registers.h"
#include "stdfunc.h"

#include <QDebug>

bool isKnownModule(quint16 mtypeb, quint16 mtypem)
{
    if (Modules::BaseBoards.contains(mtypeb))
    {
        if (mtypeb != Modules::MTM_00)
        {
            return Modules::MezzanineBoards.contains(mtypem);
        }
        return true;
    }
    return false;
}

Board::Board(Singleton::token)
{
    m_interfaceType = Unknown;
    m_connectionState = ConnectionState::Closed;
    m_boardType = Types::None;
    m_deviceType = DeviceType::Module;
    //    Q_UNUSED(parent)
}

quint16 Board::typeB() const
{
    return (m_baseSerialInfo.MTypeB >> 16);
}

// void Board::setTypeB(const quint16 &typeB)
//{
//    m_typeB = typeB;
//    emit typeChanged();
//}

quint16 Board::typeM() const
{
    return (m_baseSerialInfo.MTypeM >> 16);
}

// void Board::setTypeM(const quint16 &typeM)
//{
//    m_typeM = typeM;
//    emit typeChanged();
//}

quint16 Board::type() const
{
    const quint16 Mtypem = typeM();
    const quint16 Mtypeb = typeB() >> 8;
    return quint16(Mtypeb + Mtypem);
}

quint16 Board::type(Board::Types type) const
{
    switch (type)
    {
    case Base:
        return typeB();
    case Mezzanine:
        return typeM();
    case None:
        return 0;
    default:
        return Board::type();
    }
}

quint32 Board::serialNumber(Board::Types type) const
{
    switch (type)
    {
    case Base:
        return m_baseSerialInfo.SerialNumB;
    case Mezzanine:
        return m_baseSerialInfo.SerialNumM;
    case None:
        return 0;
    default:
        return m_baseSerialInfo.SerialNum;
    }
}

quint32 Board::UID(Range ran) const
{
    switch (ran)
    {
    case High:
        return m_baseSerialInfo.UIDHigh;
    case Mid:
        return m_baseSerialInfo.UIDMid;
    case Low:
        return m_baseSerialInfo.UIDLow;
    default:
        return 0;
    }
}

Board::InterfaceType Board::interfaceType() const
{
    return m_interfaceType;
}

void Board::setInterfaceType(Board::InterfaceType iface)
{
    m_interfaceType = iface;
    emit interfaceTypeChanged(iface);
}

Board::ConnectionState Board::connectionState() const
{
    return m_connectionState;
}

void Board::setConnectionState(ConnectionState connectionState)
{
    if (m_connectionState == connectionState && m_connectionState == ConnectionState::Connected)
        Q_ASSERT("Try to connect while still connected");
    m_connectionState = connectionState;
    emit connectionStateChanged(connectionState);
}

void Board::update(const DataTypes::BitStringStruct &bs)
{
    quint32 &item = *(reinterpret_cast<quint32 *>(&m_baseSerialInfo) + (bs.sigAdr - BSIREG));
    std::copy_n(&bs.sigVal, sizeof(quint32), &item);

    if (&item == &m_baseSerialInfo.Hth)
        emit healthChanged(m_baseSerialInfo.Hth);
}

quint32 Board::health() const
{
    return m_baseSerialInfo.Hth;
}

bool Board::noConfig() const
{
    return (health() & HTH_CONFIG) || StdFunc::IsInEmulateMode();
}

bool Board::noRegPars() const
{
    return health() & HTH_REGPARS;
}

Modules::BaseSerialInfo Board::baseSerialInfo() const
{
    return m_baseSerialInfo;
}

// QList<quint16> Board::getBaseBoardsList() const
//{
//    QList<quint16> list = Modules::BaseBoards.keys();
//    return list;
//}

Board::Types Board::boardType() const
{
    return m_boardType;
}

void Board::setBoardType(const Types &boardType)
{
    m_boardType = boardType;
    emit boardTypeChanged(boardType);
}

Board::DeviceType Board::deviceType() const
{
    return m_deviceType;
}

void Board::setDeviceType(const DeviceType &deviceType)
{
    m_deviceType = deviceType;
    emit deviceTypeChanged(deviceType);
}
