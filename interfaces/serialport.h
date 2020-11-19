#ifndef SERIALPORT_H
#define SERIALPORT_H

#include "settingstypes.h"

#include <QPointer>
#include <QSerialPort>

class SerialPort : public QObject
{

    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();
    bool Init(SerialPortSettings settings);

signals:
    void Read(QByteArray);

    void Reconnect();

private:
    QPointer<QSerialPort> Port;

public slots:
    void WriteBytes(QByteArray ba);
    void Disconnect();

private slots:
    void ReadBytes();
    void ErrorOccurred(QSerialPort::SerialPortError err);
};

#endif // SERIALPORT_H
