#ifndef CONFDIALOGKDV_H
#define CONFDIALOGKDV_H

#include "../config/configkdv.h"
#include "abstractconfdialog.h"

class ConfKDVDialog : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfKDVDialog(ConfigKDV *ckdv, QWidget *parent = nullptr);
    ~ConfKDVDialog();

private:
    void Fill() override;
    void FillBack() override;
    void SetupUI() override;
    void CheckConf() override;

protected:
    QWidget *analogWidget();
    QWidget *settingWidget();
    QWidget *alarmWidget();
    QWidget *commonAlarmWidget();
    QWidget *vibrAlarmWidget();
    QWidget *hystWidget();
    QWidget *otherWidget();
    QWidget *recordWidget();
    QWidget *motorWidget();
    virtual QWidget *connectionWidget();

    void FillKdv();
    void FillBackKdv();

    ConfigKDV *ConfKDV;

private slots:

    void SetDefConf() override;
};

#endif // CONFIGDIALOGKDV_H
