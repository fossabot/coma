#pragma once
#include "../ctti/type_id.hpp"
#include "../gen/modules.h"

#include <variant>
namespace delegate
{

enum class ItemType : int
{
    ModbusItem
};

using WidgetGroup = int;

struct Widget
{
    Widget(const ctti::unnamed_type_id_t type_) : type(type_)
    {
    }
    Widget(const ctti::unnamed_type_id_t type_, const WidgetGroup group_) : type(type_), group(group_)
    {
    }
    Widget(const ctti::unnamed_type_id_t type_, const QString &desc_, const WidgetGroup group_, const QString &toolTip_)
        : type(type_), desc(desc_), group(group_), toolTip(toolTip_)
    {
    }

    Widget &merge(const Widget &rhs)
    {
        if (!rhs.desc.isEmpty())
            desc = rhs.desc;
        if (!rhs.toolTip.isEmpty())
            desc = rhs.desc;
        if (rhs.group)
            group = rhs.group;
        return *this;
    }

    ctti::unnamed_type_id_t type;
    QString desc;
    WidgetGroup group;
    QString toolTip;
};

struct DoubleSpinBoxWidget : Widget
{
    DoubleSpinBoxWidget(const ctti::unnamed_type_id_t type_, const WidgetGroup group_) : Widget(type_, group_)
    {
    }
    double min;
    double max;
    int decimals;
    DoubleSpinBoxWidget &merge(const DoubleSpinBoxWidget &rhs)
    {
        if (rhs.min)
            min = rhs.min;
        if (rhs.max)
            max = rhs.max;
        if (rhs.decimals)
            decimals = rhs.decimals;
        Widget::merge(rhs);
        return *this;
    }
};

struct Group
{
    int count;
    QStringList items;
    Group &merge(const Group &rhs)
    {
        if (rhs.count)
            count = rhs.count;
        if (!rhs.items.isEmpty())
            items = rhs.items;
        return *this;
    }
};
struct DoubleSpinBoxGroup : DoubleSpinBoxWidget, Group
{
    DoubleSpinBoxGroup(const ctti::unnamed_type_id_t type_, const WidgetGroup group_)
        : DoubleSpinBoxWidget(type_, group_)
    {
    }
    DoubleSpinBoxGroup &merge(const DoubleSpinBoxGroup &rhs)
    {
        DoubleSpinBoxWidget::merge(rhs);
        Group::merge(rhs);
        return *this;
    }
};
struct CheckBoxGroup : Widget, Group
{
    CheckBoxGroup(const ctti::unnamed_type_id_t type_, const WidgetGroup group_) : Widget(type_, group_)
    {
    }
    CheckBoxGroup &merge(const CheckBoxGroup &rhs)
    {
        Widget::merge(rhs);
        Group::merge(rhs);
        return *this;
    }
};

struct QComboBox : Widget
{
    enum PrimaryField : int
    {
        index = 0,
        data = 1,
        bitfield = 2
    };
    QComboBox(const ctti::unnamed_type_id_t type_, const WidgetGroup group_) : Widget(type_, group_)
    {
    }
    PrimaryField primaryField = index;
    QStringList model;
    QComboBox &merge(const QComboBox &rhs)
    {
        if (!rhs.model.isEmpty())
            model = rhs.model;
        if (rhs.primaryField)
            primaryField = rhs.primaryField;
        Widget::merge(rhs);
        return *this;
    }
};

struct QComboBoxGroup : QComboBox, Group
{
    QComboBoxGroup(const ctti::unnamed_type_id_t type_, const WidgetGroup group_) : QComboBox(type_, group_)
    {
    }
    QComboBoxGroup &merge(const QComboBoxGroup &rhs)
    {
        QComboBox::merge(rhs);
        Group::merge(rhs);
        return *this;
    }
};

// Unique item, not mergeable
struct Item : Widget
{
    enum ModbusColumns : int
    {
        SensorType = 0,
        BaudRate,
        Parity,
        StopBits,
        Timeout,
        Address,
        FuncCode,
        DataType,
        Register
    };
    Item(const ctti::unnamed_type_id_t type_) : Widget(type_)
    {
    }
    Item(const ctti::unnamed_type_id_t type_, const ItemType itype_) : Widget(type_), itemType(itype_)
    {
    }
    Item(const ctti::unnamed_type_id_t type_, const ItemType itype_, const BciNumber parent_)
        : Widget(type_), itemType(itype_), parent(parent_)
    {
    }
    Item(const ctti::unnamed_type_id_t type_, const ItemType itype_, const BciNumber parent_, const WidgetGroup group_)
        : Widget(type_, group_), itemType(itype_), parent(parent_)
    {
    }
    ItemType itemType;
    BciNumber parent;
};

using itemVariant = std::variant< //
    Widget,                       //
    QComboBox,                    //
    DoubleSpinBoxGroup,           //
    DoubleSpinBoxWidget,          //
    CheckBoxGroup,                //
    Item,                         //
    QComboBoxGroup                //
    >;
}
using categoryMap = QMap<delegate::WidgetGroup, QString>;
using widgetMap = std::map<BciNumber, delegate::itemVariant>;
