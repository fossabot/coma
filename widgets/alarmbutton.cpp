#include "alarmbutton.h"

#include <QPainter>
AlarmButton::AlarmButton(QWidget *parent) : QPushButton(parent)
{
}

AlarmButton::~AlarmButton()
{
}

void AlarmButton::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
}

QSize AlarmButton::sizeHint() const
{
    const auto parentHint = QPushButton::sizeHint();

    return QSize(parentHint.width() + m_pixmap.width(), std::max(parentHint.height(), m_pixmap.height()));
}

void AlarmButton::paintEvent(QPaintEvent *e)
{
    QPushButton::paintEvent(e);

    if (!m_pixmap.isNull())
    {
        const int y = (height() - m_pixmap.height()) / 2;
        QPainter painter(this);
        painter.drawPixmap(width() / 20, y, m_pixmap);
    }
}
