#ifndef RACK_H
#define RACK_H

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include "global.h"

class Rack : public QObject
{
    Q_OBJECT
public:
    explicit Rack(QVariantList _tab, int module,  int channel, QObject *parent = 0);
    ~Rack();

    int max;
    int curr;
    QStringList tags;
    void move();
    bool isChanged();
    void apply();

signals:
    void setVolt(float value, int ch);
    void setTime(int value, int ch);

public slots:

private:
    QVariantList tab;
    bool changed;
    int mod;
    int ch;
};

#endif // RACK_H
