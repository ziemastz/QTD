#ifndef MEASUREMENTTHREAD_H
#define MEASUREMENTTHREAD_H

#include <QObject>

class MeasurementThread : public QObject
{
    Q_OBJECT
public:
    explicit MeasurementThread(QObject *parent = 0);

signals:

public slots:
};

#endif // MEASUREMENTTHREAD_H