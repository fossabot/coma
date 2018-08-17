#ifndef TRENDVIEWMODEL_H
#define TRENDVIEWMODEL_H

#include <QVector>
#include <QMap>

class TrendViewModel
{
public:
    TrendViewModel(const QStringList &dlist, const QStringList &alist, int pointsnum);
    ~TrendViewModel();

    QMap<QString, QVector<double> > AnalogMainData, DigitalMainData;
    QVector<double> MainPoints;

     // инициализация графиков
    // имена графиков контактных/аналоговых сигналов, количество точек, диапазон по оси Y для аналоговых
    void AddAnalogPoint(const QString &GraphNum, float PointValue);
    void AddDigitalPoint(const QString &GraphNum, int PointValue);
    bool SetPointsAxis(float start, float step);
    void SetFilename(const QString &fn);
    int DContains(const QString &key);
    int AContains(const QString &key);

private:
    int PointsNum;
    QString Filename;
    bool NoDiscrete, NoAnalog;
    QStringList DigitalNames, AnalogNames;

public slots:
    void SaveToExcel();
    void SaveToComtrade();
};

#endif // TRENDVIEWMODEL_H
