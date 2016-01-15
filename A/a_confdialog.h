#ifndef A_CONFDIALOG_H
#define A_CONFDIALOG_H

#include <QDialog>
#include <QGridLayout>

#include "../publicclass.h"
#include "a_config.h"

#define RT_mA          0
#define RT_V           1
#define RT_M           2

#define RT_mA420    0
#define RT_mA020    1
#define RT_mA05     2

#define RT_V05      3
#define RT_V_55     4

#define ACONFER(a)       ERMSG(publicclass::ER_ACONF,__LINE__,a)
#define ACONFDBG         DBGMSG(publicclass::ER_ACONF,__LINE__)
#define ACONFWARN        WARNMSG(publicclass::ER_ACONF,__LINE__)
#define ACONFINFO(a)     INFOMSG(publicclass::ER_ACONF,__LINE__,a)

class a_confdialog : public QDialog
{
    Q_OBJECT
public:
    explicit a_confdialog(QWidget *parent = 0);

    bool NoProperConf; // в модуле нет нормальной конфигурации

private:
    QByteArray confba;
    QGridLayout *ConfLayout;
    a_config *aconf;

    void FillConfData();
    void SetRange(int Range, int ChNum);
    bool CheckConf();
    void SetMinMax(int i);
    void SetRangeWidget(int ChNum, int RangeType);
    void SetSpinboxValue(QString name, double value);
    int GetChNumFromObjectName(QString ObjectName);

signals:
    void BsiIsNeedToBeAcquiredAndChecked();
    void ChannelDisabled(int, bool);

public slots:

private slots:
    void SetChTypData();
    void SetChOsc(int);
    void SetChOscSrc(int);
    void SetInMin();
    void SetInMax();
    void SetInVMin();
    void SetInVMax();
    void SetMinMin();
    void SetMin();
    void SetMax();
    void SetMaxMax();
    void SetRangemA();
    void SetRangeV();
    void Set104();
    void SetCType(int);
    void SetOscDly(int);
    void GetBci();
    void WriteConfDataToModule();
    void SetupUI();
    void SetNewConf();
    void SetDefConf();
    void SaveConf();
    void LoadConf();
    void DisableChannel(int ChNum, bool Disable);
    void SetRangeWidgetSlot(int RangeType);
};

#endif // A_CONFDIALOG_H
