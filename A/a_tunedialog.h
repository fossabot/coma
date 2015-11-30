#ifndef A_TUNEDIALOG_H
#define A_TUNEDIALOG_H

#include <QDialog>
//#include "canal.h"

#define TUNEFILELENGTH  256

#define ATUNEER(a)       ERMSG(publicclass::ER_ATUNE,__LINE__,a)
#define ATUNEDBG         DBGMSG(publicclass::ER_ATUNE,__LINE__)
#define ATUNEWARN        WARNMSG(publicclass::ER_ATUNE,__LINE__)
#define ATUNEINFO(a)     INFOMSG(publicclass::ER_ATUNE,__LINE__,a)

class a_tunedialog : public QDialog
{
    Q_OBJECT
public:
    explicit a_tunedialog(QWidget *parent = 0);

    bool NoProperCoefs;

signals:

public slots:
    void UpdateProper(bool);

private:
//    canal *cn;

    struct Bda
    {
        quint32 sin[16];
    };

    Bda Bda0, Bda5, Bda20;

    struct Bac
    {
        float fbin;
        float fkuin;
        float fkiin;
    };

    Bac Bac_block[16];

    void SetupUI();
    void tune(int);
    void ShowErrMsg(int);
    void CalcNewTuneCoefs();
    void RefreshTuneCoefs();
    void CheckAndShowTune0();
    void CheckAndShowTune5();
    void CheckAndShowTune20();

private slots:
    void tune0();
    void tune5();
    void tune20();
    void ReadTuneCoefs();
    void WriteTuneCoefs();
    void SaveToFile();
    void LoadFromFile();
};

#endif // A_TUNEDIALOG_H
