#ifndef BAD2_H
#define BAD2_H

//channels
#define R1 24
#define R2 36
#define DT1 60
#define DT2 165
#define DLB 90
#define ENAEXTCLK 70
#define DISEXTCLK 75

#include <QObject>
#include <QDebug>
#include <QString>
#include <QThread>
#include "FTDI/ftd2xx.h"

class BAD2 : public QObject
{
    Q_OBJECT
public:
    explicit BAD2(QObject *parent = 0);

    QVector<int> currSetTime;

    void set(QString _serialName);
    bool init();
    void deinit();

    bool setTime(int value, int ch);
    bool setCommand(int com);
    QVector<unsigned long> getData();

signals:
    void debug(QString text);
    void error(QString text);

public slots:

private:
    FT_HANDLE ftHandle;
    char *serialNumber;
    bool isConnect;
    bool write(unsigned char *cmd);
    QString errorString(FT_STATUS status);
};

#endif // BAD2_H
