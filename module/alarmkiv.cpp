#include "alarmkiv.h"

#include "module_kiv.h"
CritKIV::CritKIV(QWidget *parent) : ModuleAlarm(parent)
{
    m_startAlarmAddress = 3011;
    m_alarmFlags = std::bitset<32>(AVM_KIV::criticalFlags);
    m_alarmAllCounts = AVM_KIV::criticalEvents.size() + AVM_KIV::warningEvents.size();
    setupUI(AVM_KIV::criticalEvents);
}

CritKIV::CritKIV(const DataTypes::Alarm &desc, QWidget *parent)
{
    m_startAlarmAddress = 3011;
    m_alarmFlags = std::bitset<sizeof(desc.flags) * 8>(desc.flags);
    m_alarmAllCounts = AVM_KIV::criticalEvents.size() + AVM_KIV::warningEvents.size();
    setupUI(desc.desc);
}

WarnKIV::WarnKIV(QWidget *parent) : ModuleAlarm(parent)
{
    m_startAlarmAddress = 3011;
    m_alarmFlags = std::bitset<32>(AVM_KIV::warningFlags);
    m_alarmColor = Qt::yellow;
    m_alarmAllCounts = AVM_KIV::criticalEvents.size() + AVM_KIV::warningEvents.size();
    setupUI(AVM_KIV::warningEvents);
}

WarnKIV::WarnKIV(const DataTypes::Alarm &desc, QWidget *parent)
{
    m_startAlarmAddress = 3011;
    m_alarmFlags = std::bitset<sizeof(desc.flags) * 8>(desc.flags);
    m_alarmColor = Qt::yellow;
    m_alarmAllCounts = AVM_KIV::criticalEvents.size() + AVM_KIV::warningEvents.size();
    setupUI(desc.desc);
}
