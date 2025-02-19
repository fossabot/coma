#pragma once
#include "../config/delegate.h"
#include "../gen/modules.h"
#include "datatypes.h"
struct InterfaceSettings
{
    QVariant settings;
};

struct ModuleSettings
{
    int alarmCount() const
    {
        int counter = 0;
        for (const auto &alarm : alarms)
            counter += alarm.desc.size();
        return counter;
    }
    QMap<Modules::AlarmType, DataTypes::Alarm> alarms;
    QMap<Modules::JournalType, DataTypes::JournalDesc> journals;
    InterfaceSettings ifaceSettings;
    QList<DataTypes::RecordPair> configSettings;
    QMultiMap<quint32, quint32> highlightWarn, highlightCrit;
};

struct GlobalSettings
{
    S2DataTypes::valueTypeMap *s2filesMap;
    widgetMap *s2widgetMap;
    categoryMap *s2categories;
};

namespace settings
{
constexpr char logKey[] = "WriteLog";
constexpr char logWidget[] = "writelogchb";
constexpr char tuneCountKey[] = "TuneRequestCount";
constexpr char tuneCountWidget[] = "reqcount";
constexpr char timezoneKey[] = "Timezone";
constexpr char timezoneWidget[] = "timezoneCB";
}
