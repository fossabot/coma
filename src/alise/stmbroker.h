#pragma once
#include "../gen/modules.h"
#include "protos.pb.h"

#include <QObject>
#include <QTimer>

//#define TEST_INDICATOR

class Protocom;

class StmBroker : public QObject
{
    //    Q_OBJECT
public:
    StmBroker(QObject *parent = nullptr);
    bool connectToStm();

public /*slots*/:
    void checkPowerUnit();
    void setIndication(alise::Health_Code code);

    void setTime(timespec time);
    void getTime();

private:
    AVTUK_14::Indication transform(alise::Health_Code code) const;
    timespec transform(google::protobuf::Timestamp timestamp) const
    {
        return timespec { timestamp.seconds(), timestamp.nanos() };
    }
    // Controller m_controller;
    Protocom *m_interface;
    QTimer m_timer;
#ifdef TEST_INDICATOR
    QTimer m_testTimer;
#endif
};
