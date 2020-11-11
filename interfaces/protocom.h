#pragma once

#include "baseinterface.h"
#include "defines.h"

class Protocom : public BaseInterface
{
    Q_OBJECT
public:
    explicit Protocom(QObject *parent = nullptr);

    bool start(const ConnectStruct &st) override;
    void stop() override;

    void reqTime() override;
    void reqFile(quint32 filenum, bool isConfigFile = false) override;
    void reqStartup(quint32 sigAdr, quint32 sigCount) override;
    void reqBSI() override;

    void writeFile(quint32 filenum, const QByteArray &file) override;
    void writeTime(quint32 time) override;
    void writeCommand(Queries::Commands cmd, QList<DataTypes::SignalsStruct> list) override;
    void reqFloats(quint32 sigAdr, quint32 sigCount) override;

private:
    // Return 0 if not exist
    static CN::Commands translate(const Queries::Commands cmd)
    {
        return m_dict.value(cmd);
    }
    // Return 0 if not exist
    static Queries::Commands translate(const CN::Commands cmd)
    {
        return m_dict.key(cmd);
    }

    const static QMap<Queries::Commands, CN::Commands> m_dict;
};
