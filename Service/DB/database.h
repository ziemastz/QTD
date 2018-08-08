#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QStringList>
#include <QDir>
#include <QDateTime>
#include <Service/DB/tabdb.h>
#include <global.h>


class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QString location, QObject *parent = 0);

    QList<QStringList> registerMeasurement(QString search = QString());
    QList<QStringList> registerUser();

    QString registerMeasurementCount();

    QStringList nuclideList();
    QStringList seriesSourcesList(QString nuclide = QString());
    QStringList parametersList(QString nuclide = QString());
    QStringList userList();
    QStringList loginList();
    QStringList solutionList(QString nuclide = QString());
    QStringList scintillatorList();
    QStringList scintillatorVolumeList();
    QStringList vialTypeList();

    QString userPassword(QString login);

    USERDATA user(QString login);
    SERIESSOURCESDATA seriesSources(QString name);
    PARAMETERSDATA parameters(QString name);
    MEASUREMENTDATA measurement(QString id);

    bool insertUser(USERDATA data);
    bool insertSeriesSources(SERIESSOURCESDATA data);
    bool insertParameters(PARAMETERSDATA data);
    bool insertMeasurement(MEASUREMENTDATA data);

    bool updateMeasurement(MEASUREMENTDATA data);
    bool updateUser(USERDATA data);

    bool removeUser(QString login);
    bool removeSeriesSources(QString name);
    bool removeParameters(QString name);

signals:

public slots:

private:
    TabDB *measDB,*soDB,*paramDB,*userDB;


};

#endif // DATABASE_H
