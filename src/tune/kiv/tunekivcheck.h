#ifndef TUNEKIVCHECK_H
#define TUNEKIVCHECK_H

#include "../abstracttunedialog.h"

#include <QShowEvent>
#include <QWidget>

class TuneKIVCheck : public AbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVCheck(int tuneStep, QWidget *parent = nullptr);

private:
    void setMessages() override;
    void setTuneFunctions() override;
    Error::Msg showScheme();
    Error::Msg check();
private slots:

protected:
    void showEvent(QShowEvent *e) override;
};

#endif // TUNEKIVCHECK_H
