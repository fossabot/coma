#include <QCoreApplication>
#include <QTime>
#include "eusbhid.h"
#include "error.h"
#include "stdfunc.h"

EUsbHid::EUsbHid(QObject *parent) : EAbstractProtocomChannel(parent)
{
    ThreadRunning = false;
}

EUsbHid::~EUsbHid()
{
}

bool EUsbHid::Connect()
{
    if (Connected)
        Disconnect();
    UThread = new EUsbThread(DevInf, CnLog, IsWriteUSBLog());
    connect(UThread,SIGNAL(NewDataReceived(QByteArray)),this,SLOT(ParseIncomeData(QByteArray)));
    connect(this,SIGNAL(StopUThread()),UThread,SLOT(Finish()));
    if (UThread->Set() != Error::ER_NOERROR)
        return false;
    Connected = true;
    QTimer *tmr = new QTimer;
    tmr->setInterval(UH_MAINLOOP_DELAY);
    connect(tmr,SIGNAL(timeout()),UThread,SLOT(Run()));
    tmr->start();
    connect(this,SIGNAL(StopUThread()),tmr,SLOT(stop()));
    ThreadRunning = true;
    return true;
}

QByteArray EUsbHid::RawRead(int bytes)
{
    Q_UNUSED(bytes);
    return QByteArray();
}

qint64 EUsbHid::RawWrite(QByteArray &ba)
{
    if (!ThreadRunning)
        return Error::ER_GENERALERROR;
    qint64 res = UThread->WriteData(ba);
    if (res < 0)
        return Error::ER_GENERALERROR;
    return res;
}

void EUsbHid::RawClose()
{
    if (ThreadRunning)
    {
        emit StopUThread();
        ThreadRunning = false;
    }
    Connected = false;
}

void EUsbHid::SetDeviceInfo(int venid, int prodid, const QString &sn)
{
    DevInf.vendor_id = venid;
    DevInf.product_id = prodid;
    sn.toWCharArray(DevInf.serial);
}

EUsbThread::EUsbThread(EAbstractProtocomChannel::DeviceConnectStruct &devinfo, Log *logh, bool writelog, QObject *parent) : QObject(parent)
{
    log = logh;
    AboutToFinish = false;
    HidDevice = 0;
    WriteUSBLog = writelog;
    DeviceInfo = devinfo;
}

EUsbThread::~EUsbThread()
{
}

int EUsbThread::Set()
{
    if ((DeviceInfo.product_id == 0) || (DeviceInfo.vendor_id == 0))
        return Error::ER_GENERALERROR;
    HidDevice = hid_open(DeviceInfo.vendor_id, DeviceInfo.product_id, DeviceInfo.serial);
    if (!HidDevice)
        return Error::ER_GENERALERROR;
    hid_set_nonblocking(HidDevice, 1);
    return Error::ER_NOERROR;
}

void EUsbThread::Run()
{
    try
    {
        unsigned char data[UH_MAXSEGMENTLENGTH+1]; // +1 to ID
        if (!AboutToFinish)
        {
            // check if there's any data in input buffer
            int bytes;
            if (HidDevice != 0)
            {
                bytes = hid_read(HidDevice, data, UH_MAXSEGMENTLENGTH+1);
                if (bytes < 0)
                {
                    if (WriteUSBLog)
                        log->WriteRaw("UsbThread: Unable to hid_read()");
                    AboutToFinish = true;
                    emit Finished();
                    return;
                }
                if (bytes > 0)
                {
                    QByteArray ba(reinterpret_cast<char*>(data), bytes);
                    emit NewDataReceived(ba);
                }
/*            QTime tme;
            tme.start();
            while (tme.elapsed() < UH_MAINLOOP_DELAY)
                QCoreApplication::processEvents(QEventLoop::AllEvents);
            RunMutex.lock();
            QWC.wait(&RunMutex);
            RunMutex.unlock(); */
            }
        }
    }
    catch(...)
    {
        emit Finished();
    }
}

qint64 EUsbThread::WriteData(QByteArray &ba)
{
    if (HidDevice != 0)
    {
        if (ba.size() > UH_MAXSEGMENTLENGTH)
        {
            if (WriteUSBLog)
                log->WriteRaw("UsbThread: WRONG SEGMENT LENGTH!\n");
            ERMSG("Длина сегмента больше "+QString::number(UH_MAXSEGMENTLENGTH)+" байт");
            return Error::ER_GENERALERROR;
        }
        if (ba.size() < UH_MAXSEGMENTLENGTH)
            ba.append(UH_MAXSEGMENTLENGTH - ba.size(), static_cast<char>(0x00));
        ba.prepend(static_cast<char>(0x00)); // inserting ID field
        if (WriteUSBLog)
        {
            QByteArray tmpba = "UsbThread: ->" + ba.toHex() + "\n";
            log->WriteRaw(tmpba);
        }
        return hid_write(HidDevice, reinterpret_cast<unsigned char *>(ba.data()), ba.size());
    }
    return 0;
}

void EUsbThread::Finish()
{
    if (!AboutToFinish)
    {
        if (HidDevice != 0)
        {
            hid_close(HidDevice);
            HidDevice = 0;
        }
        emit Finished();
        AboutToFinish = true;
    }
}
