#ifndef REPORT_H
#define REPORT_H

#include <QObject>
#include <QDebug>
#include <QDateTime>
#include <QVector>
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>
#include <QTextStream>
#include <QDir>
#include <QMap>

#include "global.h"

class List {
public:
    QList<QString> keys;
    QList<QString> values;
    void insert(QString key, QString value);
    void clear();
};

class Report : public QObject
{
    Q_OBJECT
public:
    explicit Report(MEASUREMENTDATA measData, SERIESSOURCESDATA seriesData, PARAMETERSDATA paramData, QString location, QObject *parent = 0);
    void insertRecord(QString hour, int source, int point, QString tagPoint, int reps, QVector<unsigned long> counters);

    static void appendText(QString filename, QString text);
    static void appendMapsList(QString filename, List map);
    static void appendTable(QString filename, QList<QStringList> table);

signals:

public slots:
    void debug(QString text);
    void addCurrentStatusCounters(QVector<unsigned long> counters);
    void addCurrentStatusHV(QVector<float> volt, QVector<float> amp);
private:
    QString filenameRT_TD;
    QString filenameRT_QTD;
    QString filenamePM_RAW;
    QString filenameRT_RES;
    QString filenamePM_RES;
    QString blankTime,sourceTime,totalReps;
    QStringList masses;
    int currentSource;
    QString filenameDebug;
    QString filenameHV;
    QString filenameAnalyzer;

};

#endif // REPORT_H
