#pragma once

#include "../gen/pch.h"
struct UsbHidSettings
{
    explicit UsbHidSettings() = default;
    explicit UsbHidSettings(unsigned short dev, unsigned short pid, const QString &arr, const QString &str)
        : vendor_id(dev)
        , product_id(pid)
        , serial(arr)
#if _DEBUG
        , path(str)
#endif
    {
    }
    // Не сравниваем path т.к. если переподключить устройство то path может сменится но остальные параметры - нет
    friend bool operator==(const UsbHidSettings &lhs, const UsbHidSettings &rhs)
    {
        return ((lhs.product_id == rhs.product_id) && (lhs.vendor_id == rhs.vendor_id) && (lhs.serial == rhs.serial));
    }
    friend bool operator!=(const UsbHidSettings &lhs, const UsbHidSettings &rhs)
    {
        return !(lhs == rhs);
    }

    unsigned short vendor_id;
    unsigned short product_id;
    QString serial;
#if _DEBUG
    QString path;
#endif
    /// From Windows message string
    bool hasMatch(const QString &str);
    bool hasPartialMatch(const QString &str);
};

Q_DECLARE_METATYPE(UsbHidSettings)
