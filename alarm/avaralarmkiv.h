#ifndef AVARALARMKIV_H
#define AVARALARMKIV_H

#include "../alarm/abstractalarm.h"

class AvarAlarmKIV: public AbstractAlarm
{
   Q_OBJECT

public:
   explicit  AvarAlarmKIV(QWidget *parent = nullptr);


public slots:

   void AlarmState();
   void PredAlarmState();
   void AvarState();

   void UpdateUSB();
   void USBSetAlarms();

   void UpdatePredAlarmEvents(IEC104Thread::SponSignals *);
   void UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *);

   void ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal);
   void ModBusUpdatePredAlarmEvents(ModBus::Coils Signal);

private:

   QWidget *Wpred;
   QWidget *Walarm;



};

#endif // AVARALARM84_H
