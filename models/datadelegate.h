#ifndef CHECKDELEGATE_H
#define CHECKDELEGATE_H

#include "../gen/datablock.h"
#include "../gen/error.h"
#include <QStyledItemDelegate>
#include <QWidget>

class DataDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    DataDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

private:
    //    mutable DataBlock::DataStyles m_delegateType;
    //    mutable DataBlock::DataFormat m_dataFormat;

    //    Error::Msg getStyleAndFormat(const QModelIndex &index) const;
};

#endif // CHECKDELEGATE_H
