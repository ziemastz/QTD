#include "database.h"

Database::Database(QString location, QObject *parent) : QObject(parent)
{
    if(location.isEmpty())
        location = QDir::currentPath();

    QDir dirDB(location+"/DB");
    if(!dirDB.exists()) {
        if(!dirDB.mkpath(dirDB.path())) {
            dirDB.setPath(QDir::currentPath());
            qWarning() << "Error creating database folder. Current path of database: "+dirDB.path();
        }
    }

    userDB = new TabDB(dirDB.path()+"/user.tab",userHeaders);
    soDB = new TabDB(dirDB.path()+"/source.tab",sourceHeaders);
    paramDB = new TabDB(dirDB.path()+"/parameter.tab",parameterHeaders);
    measDB = new TabDB(dirDB.path()+"/measurement.tab",measurementHeaders);
}

QList<QStringList> Database::registerMeasurement(QString search)
{
    QList<QStringList> ret,tmp;

    if(search.isEmpty()) {
        //select all
        measDB->select(&tmp);
    }else {
        measDB->select(search, &tmp);
    }
    for(int i=tmp.count()-1;i>=0;i--)
        ret << tmp.at(i);
    return ret;
}

QList<QStringList> Database::registerUser()
{
    QList<QStringList> ret;
    userDB->select(&ret);
    return ret;
}

QString Database::registerMeasurementCount()
{
    int cnt = 0;
    measDB->count(&cnt);
    return QString::number(cnt);
}

QStringList Database::nuclideList()
{
    QStringList ret,tmp;

    measDB->select(M_NUCLIDE,&tmp);
    foreach (QString var, tmp) {
        if(!var.isEmpty() && !ret.contains(var,Qt::CaseInsensitive))
            ret << var;
    }
    soDB->select(S_NUCLIDE,&tmp);
    foreach (QString var, tmp) {
        if(!var.isEmpty() && !ret.contains(var,Qt::CaseInsensitive))
            ret << var;
    }
    paramDB->select(P_NUCLIDE,&tmp);
    foreach (QString var, tmp) {
        if(!var.isEmpty() && !ret.contains(var,Qt::CaseInsensitive))
            ret << var;
    }

    ret.sort();
    return ret;
}

QStringList Database::seriesSourcesList(QString nuclide)
{
    QStringList ret,tmp;
    if(nuclide.isEmpty()) {
        soDB->select(S_NAME,&tmp);
    }else {
        soDB->select(S_NAME,S_NUCLIDE,nuclide,&tmp);
    }
    tmp.removeDuplicates();
    for(int i=tmp.count()-1;i>=0;i--)
        ret << tmp.at(i);

    return ret;
}

QStringList Database::parametersList(QString nuclide)
{
    QStringList ret,tmp;
    if(nuclide.isEmpty()) {
        paramDB->select(P_NAME,&tmp);
    }else {
        paramDB->select(P_NAME,P_NUCLIDE,nuclide,&tmp);
    }
    tmp.removeDuplicates();
    for(int i=tmp.count()-1;i>=0;i--)
        ret << tmp.at(i);

    return ret;
}

QStringList Database::userList()
{
    QStringList ret,tmp;
    QList<QStringList> records;

    userDB->select(&records);
    foreach (QStringList record, records) {
        if(record.at(U_NAME) != "admin")
            ret << QString(record.at(U_NAME).at(0))+". "+record.at(U_SURNAME);
    }
    measDB->select(M_MADEBY,&tmp);
    ret << tmp;
    soDB->select(S_PREPAREDBY,&tmp);
    ret << tmp;

    ret.removeDuplicates();
    ret.sort();
    return ret;
}

QStringList Database::loginList()
{
    QStringList ret;
    userDB->select(U_LOGIN,&ret);
    return ret;
}

QStringList Database::solutionList(QString nuclide)
{
    QStringList ret,tmp,tmp2;
    if(nuclide.isEmpty()) {
        soDB->select(S_SOLUTION,&tmp);
    }else {
        soDB->select(S_SOLUTION,P_NUCLIDE,nuclide,&tmp);
    }

    if(nuclide.isEmpty()) {
        soDB->select(S_MOTHERSOLUTION,&tmp2);
    }else {
        soDB->select(S_MOTHERSOLUTION,P_NUCLIDE,nuclide,&tmp2);
    }
    tmp << tmp2;
    tmp.removeDuplicates();
    tmp.sort();
    for(int i=tmp.count()-1;i>=0;i--)
        ret << tmp.at(i);
    return ret;
}

QStringList Database::scintillatorList()
{
    QStringList ret;
    soDB->select(S_SCINTNAME,&ret);
    ret.removeDuplicates();
    ret.sort();
    return ret;
}

QStringList Database::scintillatorVolumeList()
{
    QStringList ret;
    soDB->select(S_SCINTVOLUME,&ret);
    ret.removeDuplicates();
    ret.sort();
    return ret;
}

QStringList Database::vialTypeList()
{
    QStringList ret;
    soDB->select(S_VIALTYPE,&ret);
    ret.removeDuplicates();
    ret.sort();
    return ret;
}

USERDATA Database::user(QString login)
{
    USERDATA ret;
    QList<QStringList> records;
    userDB->select(U_LOGIN,login,&records);
    if(!records.isEmpty()) {
        QStringList record = records.last();
        ret.login = record.at(U_LOGIN);
        ret.password = record.at(U_PASSWORD);
        ret.name = record.at(U_NAME);
        ret.surname = record.at(U_SURNAME);
        ret.lastModification = record.at(U_LASTMODIFICATION);
    }

    return ret;
}

QString Database::userPassword(QString login)
{
    QStringList ret;
    userDB->select(U_PASSWORD,U_LOGIN,login,&ret);
    if(ret.isEmpty())
        return QString();
    else
        return ret.last();
}

SERIESSOURCESDATA Database::seriesSources(QString name)
{
    SERIESSOURCESDATA ret;
    QList<QStringList> records;

    soDB->select(S_NAME,name,&records);

    if(!records.isEmpty()) {
        QStringList lastRecord = records.last();

        ret.name = lastRecord.at(S_NAME);
        ret.nuclide = lastRecord.at(S_NUCLIDE);
        ret.solution = lastRecord.at(S_SOLUTION);
        ret.dilution = lastRecord.at(S_DILUTION).toDouble();
        ret.motherSolution = lastRecord.at(S_MOTHERSOLUTION);
        ret.nameScintillator = lastRecord.at(S_SCINTNAME);
        ret.volumeScintillator = lastRecord.at(S_SCINTVOLUME);
        ret.vialType = lastRecord.at(S_VIALTYPE);
        ret.idVials = lastRecord.at(S_IDVIALS).split(" ");
        QStringList masses = lastRecord.at(S_MASSES).split(" ");
        foreach (QString val, masses) {
           ret.masses << val.toDouble();
        }
        ret.preparedBy = lastRecord.at(S_PREPAREDBY);
        ret.preparedAt = QDate::fromString(lastRecord.at(S_PREPAREDAT),formatDate);
        ret.lastModification = lastRecord.at(S_LASTMODIFICATION);

    }
    return ret;
}

PARAMETERSDATA Database::parameters(QString name)
{
    PARAMETERSDATA ret;
    QList<QStringList> records;

    paramDB->select(P_NAME,name,&records);

    if(!records.isEmpty()) {
        QStringList lastRecord = records.last();
        ret.name = lastRecord.at(P_NAME);
        ret.nuclide = lastRecord.at(P_NUCLIDE);
        QStringList tmp;

        tmp = lastRecord.at(P_HVA1).split(" ");
        foreach (QString val, tmp) {
            ret.hvAnodeBeta << val.toDouble();
        }
        tmp = lastRecord.at(P_HVD1).split(" ");
        foreach (QString val, tmp) {
            ret.hvFirstDynodeBeta << val.toDouble();
        }
        tmp = lastRecord.at(P_HVF).split(" ");
        foreach (QString val, tmp) {
            ret.hvFocalysingDynodeBeta << val.toDouble();
        }
        tmp = lastRecord.at(P_HVA2).split(" ");
        foreach (QString val, tmp) {
            ret.hvAnodeGamma << val.toDouble();
        }

        tmp = lastRecord.at(P_R1).split(" ");
        foreach (QString val, tmp) {
            ret.resolvintTimeBeta << val.toDouble();
        }
        tmp = lastRecord.at(P_DT1).split(" ");
        foreach (QString val, tmp) {
            ret.deadTimeBeta << val.toDouble();
        }
        tmp = lastRecord.at(P_R2).split(" ");
        foreach (QString val, tmp) {
            ret.resolvingTimeGamma << val.toDouble();
        }
        tmp = lastRecord.at(P_DT2).split(" ");
        foreach (QString val, tmp) {
            ret.deadTimeGamma << val.toDouble();
        }
        tmp = lastRecord.at(P_DLB).split(" ");
        foreach (QString val, tmp) {
            ret.delayTimeBeta << val.toDouble();
        }
        ret.upperWindowGamma = lastRecord.at(P_GWUPPER).toInt();
        ret.lowerWindowGamma = lastRecord.at(P_GWLOWER).toInt();
        ret.timeLimited = static_cast<TIMELIMITED>(lastRecord.at(P_TIME).toInt());
        ret.valueCountLimited = lastRecord.at(P_LCCOUNTS).toInt();
        ret.valueUncertaintlyLimited = lastRecord.at(P_LCUNCERTAINTY).toDouble();
        ret.channelLimited = static_cast<CHANNELLIMITED>(lastRecord.at(P_LCCHANNEL).toInt());
        ret.lastModification = lastRecord.at(P_LASTMODIFICATION);
    }
    return ret;
}

MEASUREMENTDATA Database::measurement(QString id)
{
    MEASUREMENTDATA ret;
    QList<QStringList> records;

    measDB->select(M_ID,id,&records);

    if(!records.isEmpty()) {
        QStringList lastRecord = records.last();
        ret.id = lastRecord.at(M_ID);
        ret.nuclide = lastRecord.at(M_NUCLIDE);
        ret.filename = lastRecord.at(M_FILENAME);
        ret.mode = lastRecord.at(M_MODE).toInt();
        ret.date = lastRecord.at(M_DATE);
        ret.nameSeriesSources = lastRecord.at(M_SERIESSOURCES);
        ret.nameParameters = lastRecord.at(M_PARAMETERS);
        ret.backgroundTime = lastRecord.at(M_BACKGROUNDTIME).toInt();
        ret.sourceTime = lastRecord.at(M_SOURCETIME).toInt();
        ret.reps = lastRecord.at(M_REPS).toInt();
        ret.comment = lastRecord.at(M_COMMENT);
        ret.madeBy = lastRecord.at(M_MADEBY);
        ret.status = lastRecord.at(M_STATUS).toInt();
        ret.lastModification = lastRecord.at(M_LASTMODIFICATION);
    }
    return ret;
}

bool Database::insertUser(USERDATA data)
{
    QStringList record;
    record << data.login
           << data.password
           << data.name
           << data.surname
           << data.lastModification;
    return userDB->insert(record);
}

bool Database::insertSeriesSources(SERIESSOURCESDATA data)
{
    QStringList record;
    record << data.name
           << data.nuclide
           << data.solution
           << QString::number(data.dilution,'g',10)
           << data.motherSolution
           << data.nameScintillator
           << data.volumeScintillator
           << data.vialType
           << data.idVials.join(" ");
    QStringList masses;
    foreach (double val, data.masses) {
        masses << QString::number(val);
    }
    record << masses.join(" ")
           << data.preparedBy
           << data.preparedAt.toString(formatDate)
           << data.lastModification;
    return soDB->insert(record);
}

bool Database::insertParameters(PARAMETERSDATA data)
{
    QStringList record;
    record << data.name
           << data.nuclide;
    QStringList tmp;
    foreach (float val, data.hvAnodeBeta) {
        tmp << QString::number(val);
    }
    record << tmp.join(" ");
    tmp.clear();
    foreach (float val, data.hvFirstDynodeBeta) {
        tmp << QString::number(val);
    }
    record << tmp.join(" ");
    tmp.clear();
    foreach (float val, data.hvFocalysingDynodeBeta) {
        tmp << QString::number(val);
    }
    record << tmp.join(" ");
    tmp.clear();
    foreach (float val, data.hvAnodeGamma) {
        tmp << QString::number(val);
    }
    record << tmp.join(" ");
    tmp.clear();
    foreach (int val, data.resolvintTimeBeta) {
        tmp << QString::number(val);
    }
    record << tmp.join(" ");
    tmp.clear();
    foreach (int val, data.deadTimeBeta) {
        tmp << QString::number(val);
    }
    record << tmp.join(" ");
    tmp.clear();
    foreach (int val, data.resolvingTimeGamma) {
        tmp << QString::number(val);
    }
    record << tmp.join(" ");
    tmp.clear();
    foreach (int val, data.deadTimeGamma) {
        tmp << QString::number(val);
    }
    record << tmp.join(" ");
    tmp.clear();
    foreach (int val, data.delayTimeBeta) {
        tmp << QString::number(val);
    }
    record << tmp.join(" ")
           << QString::number(data.lowerWindowGamma)
           << QString::number(data.upperWindowGamma)
           << QString::number(data.channelLimited)
           << QString::number(data.valueCountLimited)
           << QString::number(data.valueUncertaintlyLimited)
           << QString::number(data.timeLimited)
           << data.lastModification;
    return paramDB->insert(record);
}

bool Database::insertMeasurement(MEASUREMENTDATA data)
{
    if(data.id.isEmpty() || data.nuclide.isEmpty())
        return false;
    QStringList record;
    record << data.id
           << data.nuclide
           << data.filename
           << QString::number(data.mode)
           << data.date
           << data.nameSeriesSources
           << data.nameParameters
           << QString::number(data.backgroundTime)
           << QString::number(data.sourceTime)
           << QString::number(data.reps)
           << data.comment
           << data.madeBy
           << QString::number(data.status)
           << data.lastModification;
    return measDB->insert(record);
}

bool Database::updateMeasurement(MEASUREMENTDATA data)
{
    if(data.id.isEmpty() || data.nuclide.isEmpty())
        return false;
    QStringList record;
    record << data.id
           << data.nuclide
           << data.filename
           << QString::number(data.mode)
           << data.date
           << data.nameSeriesSources
           << data.nameParameters
           << QString::number(data.backgroundTime)
           << QString::number(data.sourceTime)
           << QString::number(data.reps)
           << data.comment
           << data.madeBy
           << QString::number(data.status)
           << data.lastModification;
    return measDB->update(data.id,record);
}

bool Database::updateUser(USERDATA data)
{
    QStringList record;
    record << data.login
           << data.password
           << data.name
           << data.surname
           << data.lastModification;
    return userDB->update(data.login,record);
}

bool Database::removeUser(QString login)
{
    return userDB->remove(U_LOGIN,login);
}

bool Database::removeSeriesSources(QString name)
{
    return soDB->remove(S_NAME,name);
}

bool Database::removeParameters(QString name)
{
    return paramDB->remove(P_NAME,name);
}
