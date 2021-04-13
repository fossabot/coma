#ifndef CONFDIALOGKIV_H
#define CONFDIALOGKIV_H

#include "abstractconfdialog.h"

#include <set>
class ConfigKIV;
class ConfKIVDialog : public AbstractConfDialog
{

public:
    explicit ConfKIVDialog(ConfigKIV *ckiv, QWidget *parent = nullptr);
    ~ConfKIVDialog();

private:
    // void Fill() override;

    // void SetupUI() override;
    void CheckConf() const override;

protected:
    //    QWidget *analogWidget();
    //    QWidget *thresholdsWidget();
    //    QWidget *remainsWidget();
    //    virtual QWidget *connectionWidget();
    //    QWidget *testWidget();

    //    void FillKiv();
    //    void FillBackKiv() const;

    ConfigKIV *CKIV;

private:
    void SetDefConf() override;
};

#endif // CONFDIALOG84_H
