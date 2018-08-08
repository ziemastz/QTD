#ifndef ANALYZER_H
#define ANALYZER_H

#include <QObject>
#include <QVector>
#include <QTimer>

#include "global.h"
#include "Service/Analyzer/bad1.h"
#include "Service/Analyzer/bad2.h"

class Analyzer : public QObject
{
    Q_OBJECT
public:
    explicit Analyzer(QObject *parent = 0);
    ~Analyzer();
signals:
    void started();
    void finished();
    void stop();
    void debug(QString text);
    void error(QString text);
    void status(QString stat, QVector<int> timeCh, QVector<unsigned long> counters);
    void tested(bool res);

public slots:
    void init();
    void set(MODULEDATA data);
    void quit();

    void setCommand(int command);
    void setTime(int value, int ch);

    void test();

private slots:
    void checkStatus();

private:
    QString name;
    BAD1 bad1;
    BAD2 bad2;
    QTimer *timer;

    QString currentStatus;
    QString monStatus();
    QVector<int> monTime();
    QVector<unsigned long> monCounters();

};

#endif // ANALYZER_H
