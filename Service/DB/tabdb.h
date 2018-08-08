#ifndef TABDB_H
#define TABDB_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QList>
#include <QStringList>
#include <QDebug>

class TabDB : public QObject
{
    Q_OBJECT
public:
    explicit TabDB(QString _filename, QStringList _headers);

    bool insert(QStringList record);
    bool insert(QList<QStringList> records);
    bool remove(QString id);
    bool remove(int where, QString value);
    bool update(QString id, QStringList updateRecord);

    bool select(QList<QStringList> *records);
    bool select(QString search, QList<QStringList> *records);
    bool select(int header, QStringList *items);
    bool select(int header, int where, QString value, QStringList *items);
    bool select(int where, QString value, QList<QStringList> *rows);
    bool count(int *no);
    bool lastID(int *id);

    QStringList getHeaders();
    QString getFilename();

private:
    QString filename;
    QStringList headers;
    QFile file;
};

#endif // TABDB_H
