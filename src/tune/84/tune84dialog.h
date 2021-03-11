#ifndef TUNE84DIALOG_H
#define TUNE84DIALOG_H

//#include "../../check/checkkiv.h"
#include "../../config/config.h"
#include "../../config/configkiv.h"
#include "../../gen/report.h"
#include "../../widgets/udialog.h"
#include "../generaltunedialog.h"
#include "../kiv/tunekivtemp60.h"
#include "tune84adc.h"
#include "tune84check.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>

class Tune84Dialog : public GeneralTuneDialog
{
    Q_OBJECT
public:
    explicit Tune84Dialog(ConfigKIV *ckiv, QWidget *parent = nullptr);

public slots:

private:
    ConfigKIV *CKIV;
    Tune84Check *T84CheckDialog;
    Tune84ADC *T84ADCDialog;
    TuneKIVTemp60 *T8460Dialog, *T8420Dialog;

    LimeReport::ReportEngine *m_Report;

    void SetupUI();

private slots:
    void GenerateReport();
};

#endif // TUNE84DIALOG_H
