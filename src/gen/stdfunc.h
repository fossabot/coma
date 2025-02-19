#ifndef STDFUNC_H
#define STDFUNC_H

#include <QObject>
#include <QString>
#include <cmath>
#include <functional>
#include <memory>
#define MAINSLEEP 20
#define MAINTIMEOUT 2000 // 2 sec timeout for general operations

namespace QtHelper
{
struct deleteLaterDeletor
{
    void operator()(QObject *object) const
    {
        if (object)
        {
            object->deleteLater();
        }
    }
};
}

template <typename T> using SharedPointer = std::shared_ptr<T>;

template <typename T> using UniquePointer = std::unique_ptr<T, QtHelper::deleteLaterDeletor>;

//// Another variant of std::unique_ptr with deleteLater
// template <typename T, std::enable_if_t<std::is_base_of<QObject, T>, bool>::value = true>
// using UniquePointer2 = std::unique_ptr<T, decltype(std::mem_fn(&QObject::deleteLater))>;

//// like std::make_unique
// template <typename T, typename... Args, std::enable_if_t<std::is_base_of<QObject, T>, bool>::value = true>
// UniquePointer2<T> create_unique(Args... args)
//{
//    return UniquePointer2<T>(new T(args...), std::mem_fn(&QObject::deleteLater));
//}

class StdFunc
{
public:
    StdFunc();

    static QString DeviceIP;
    static QString s_OrganizationString;

    static void Init();
    static QString VerToStr(quint32);
    static bool floatIsWithinLimits(double var, double base, double tolerance)
    {
        auto tmpf = fabs(var - base);
        return (tmpf < fabs(tolerance));
    }
    static float toFloat(const QString &text, bool *ok = nullptr);
    static void SetHomeDir(const QString &dir);
    static QString GetHomeDir();
    static QString GetSystemHomeDir();
    static void SetDeviceIP(const QString &ip);
    static QString ForDeviceIP();
    static void SetOrganizationString(const QString &str);
    static QString OrganizationString();
    static void setTuneRequestCount(int n);
    static int tuneRequestCount();
    static void cancel();
    static void clearCancel();
    static bool isCancelled();
    static void setCancelDisabled();
    static void setCancelEnabled();
    static bool IsInEmulateMode();
    static void SetEmulated(bool tb);
    static int IndexByBit(quint32 dword); // возвращение номера первого, начиная с младшего,
                                          // установленного бита, нумерация с 1, dword=0 => return 0
    static quint32 BitByIndex(int idx); // возвращение битовой маски по индексу (0
                                        // => 0, 1 => 1, 2 => 2, 3 => 4, ...)
                                        //    static QString PrbMessage();
                                        //    static void SetPrbMessage(const QString &msg);
    static void Wait(int ms = MAINSLEEP);

    static quint32 ping(quint32 addr);
    static quint32 checkPort(quint32 ip4Addr, quint16 port);
    template <typename T> static void joinItem(QList<T> &list, const T item)
    {
        list.append(item);
    }
    static QByteArray compress(const QByteArray &data);
    static bool checkArchiveExist(const QString &path);

    static void removeSubstr(std::string &str, std::string &substr);

    template <typename T, size_t size = sizeof(T)> static QByteArray arrayFromNumber(T number)
    {
        QByteArray ba(size, 0);
        *(reinterpret_cast<T *>(ba.data())) = number;
        return ba;
    }

    template <typename T> static QVariantList toVariantList(const QList<T> &list)
    {
        QVariantList newList;
        for (const T &item : list)
            newList.push_back(item);
        return newList;
    }
    template <typename T> static QList<T> toValuesList(const QList<T *> &list)
    {
        QList<T> newList;
        newList.reserve(list.size());
        std::transform(std::begin(list), std::end(list), std::back_inserter(newList), [](T *item) { return *item; });
        return newList;
    }

    // recursive function to count set bits
    template <typename T> static int countSetBits(T N)
    {
        int count = 0;
        for (int i = 0; i < sizeof(T) * 8; i++)
        {
            if (N & (1 << i))
                count++;
        }
        return count;
    }

private:
    static QString HomeDir;       // рабочий каталог программы
    static QString SystemHomeDir; // системный каталог программы
    static bool Emul;
    static bool Cancelled, s_cancelEnabled;
    //    static QString PrbMsg;
    static int m_tuneRequestCount; // степень усреднения для регулировки
};

namespace std_ext
{
#if defined(Q_CC_MSVC)
__forceinline
#elif defined(Q_CC_GNU) || defined(Q_CC_CLANG)
__attribute__((always_inline)) inline
#else
inline
#endif
    unsigned int
    clp2(unsigned int x)
{
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}
}

#endif // STDFUNC_H
