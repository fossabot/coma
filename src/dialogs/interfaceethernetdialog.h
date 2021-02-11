#pragma once
#include "abstractinterfacedialog.h"

#include <QProgressDialog>
class InterfaceEthernetDialog : public AbstractInterfaceDialog
{
    Q_OBJECT
public:
    explicit InterfaceEthernetDialog(QWidget *parent = nullptr);

    void setupUI() override;
    void setInterface(QModelIndex index) override;

    void addInterface() override;
    void scanInterface() override;
    void acceptedInterface() override;
    bool updateModel(QDialog *dlg) override;

private:
    QList<quint32> m_hosts;

    /// Кандидаты для отдельного класса
    QProgressDialog *m_progress;
    void handlePing();
    void handlePingFinish();
    void handlePortFinish();
    void createPingTask(quint32 ip);
    void createPortTask();
    ///

signals:
    void pingFinished();
    void modelUpdated();
};
