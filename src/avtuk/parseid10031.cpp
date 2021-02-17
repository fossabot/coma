#include "parseid10031.h"

#include "../config/config.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/timefunc.h"
#include "eoscillogram.h"
#include "parsemodule.h"
#include "trendviewdialog.h"
#include "trendviewmodel.h"

#include <QVector>

// quint32 ParseID10031::len2;

ParseID10031::ParseID10031(QByteArray &BA) : ParseModule(BA)
{
}

bool ParseID10031::Parse(int &count)
{
    QStringList tmpdv { "OCNA", "OCNB", "OCNC", "OCFA", "OCFB", "OCFC", "BKCA", "BKCB", "BKCC", "BKOA", "BKOB", "BKOC",
        "CSC", "CSO", "CNA", "CNB", "CNC", "CFA", "CFB", "CFC" };
    QStringList tmpav { "USA", "USB", "USC", "IA", "IB", "IC", "ULA", "ULB", "ULC" };

    // ParseID10031::SWJournalRecordStruct SWJ;
    // PosPlusPlus(&SWJ, count, sizeof(SWJ));
    S2DataTypes::DataRecHeader DR;
    if (!PosPlusPlus(&DR, count, sizeof(DR)))
        return false;

    S2DataTypes::OscHeader_Data OHD;
    if (!PosPlusPlus(&OHD, count, sizeof(OHD)))
        return false;
    //    ParseID10031::len2 = OHD.len;

    if (!PosPlusPlus(&DR, count, sizeof(DR)))
        return false;

    // TrendViewModel::SaveID(DR.id); // для выбора
    // составляем имя файла осциллограммы
    QString tmps = TimeFunc::UnixTime64ToString(OHD.unixtime);
    tmps.replace("/", "-");
    tmps.replace(":", "_");
    tmps.insert(0, "_");
    tmps.insert(0, QString::number(DR.id));

    TModel = new TrendViewModel(tmpdv, tmpav, OHD.len);
    TModel->SaveID(DR.id);
    TModel->Len = OHD.len;
    TModel->tmpdv_85 = tmpdv;
    TModel->tmpav_85 = tmpav;

    switch (DR.id)
    {
    case MT_ID85:
        if (!ParseID85(OHD, tmps, count))
            return false;
        break;
    }
    return true;
}

bool ParseID10031::ParseID85(S2DataTypes::OscHeader_Data &OHD, const QString &fn, int &count)
{

    float xmin = -10; //-(static_cast<float>(OHD.len/2));

    if (!TModel->SetPointsAxis(xmin, OHD.step))
        return false;
    for (quint32 i = 0; i < OHD.len; ++i) // цикл по точкам
    {
        Point85 point;
        if (!PosPlusPlus(&point, count, sizeof(Point85)))
            return false;
        //                quint32 DisPoint = point.Dis & 0x000FFFFF; // оставляем только младшие 20 бит
        quint32 DisPoint = point.Dis;
        for (int i = 0; i < TModel->tmpdv_85.size(); ++i)
        {
            if (DisPoint & 0x00000001)
                TModel->AddDigitalPoint(TModel->tmpdv_85.at(i), 1);
            else
                TModel->AddDigitalPoint(TModel->tmpdv_85.at(i), 0);
            DisPoint >>= 1;
        }
        for (int i = 0; i < TModel->tmpav_85.size(); ++i)
            TModel->AddAnalogPoint(TModel->tmpav_85.at(i), point.An[i]);
    }
    TModel->SetFilename(fn);
    return true;
}

/*void ParseID10031::Save(quint32 *len)
{
//    len = &ParseID10031::len2;
    TModel->Len =
} */
