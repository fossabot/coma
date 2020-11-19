#include "warnkdv.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../widgets/wd_func.h"

#include <QBoxLayout>

WarnKDV::WarnKDV(QWidget *parent) : Warn(parent)
{
    m_warnBdNum = 11;
    m_startWarnAddress = 3011;
    m_warnFlags = std::bitset<32>(0x1fff1ec6);

    const QStringList events = QStringList() << "Отсутствует сигнал напряжения фазы A                   "
                                             << "Отсутствует сигнал напряжения фазы B                   "
                                             << "Отсутствует сигнал напряжения фазы С                   "
                                             << "Отсутствует ток ввода фазы А (ток меньше 2мА)          "
                                             << "Отсутствует ток ввода фазы B (ток меньше 2мА)          "
                                             << "Отсутствует ток ввода фазы C (ток меньше 2мА)          "
                                             << "Не заданы начальные значения                           "
                                             << "Низкое напряжение фазы A                               "
                                             << "Низкое напряжение фазы B                               "
                                             << "Низкое напряжение фазы C                               "
                                             << "Сигнализация по приращению тангенса дельта ввода фазы А"
                                             << "Сигнализация по приращению тангенса дельта ввода фазы B"
                                             << "Сигнализация по приращению тангенса дельта ввода фазы C"
                                             << "Сигнализация по приращению C ввода фазы А              "
                                             << "Сигнализация по приращению C ввода фазы B              "
                                             << "Сигнализация по приращению C ввода фазы C              "
                                             << "Не заданы паспортные значения                          "
                                             << "Сигнализация по повышенному небалансу токов            ";

    setupUI(events);
}
