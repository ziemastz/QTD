#include "tabdb.h"

TabDB::TabDB(QString _filename, QStringList _headers)
{
    filename = _filename;
    headers = _headers;

    if(!QFile::exists(filename)) {
        QFileInfo infoFile(filename);

        QDir dir(infoFile.path());
        if(!dir.exists()) {
            if(!dir.mkpath(dir.path())) {
                qDebug() << "Error with file path: "+dir.path();

            }
        }
        file.setFileName(filename);
        if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << headers.join("|")+"\n";
            file.close();
            qDebug() << "Create tab in "+filename+" with headers: "+headers.join("|");

        }else {
            qDebug() << "Error opened "+filename+": "+file.errorString();
        }
    }else {
        file.setFileName(filename);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString headersRead = in.readLine();
            if(headersRead != headers.join("|")) {
                qDebug() << "Error with headers in "+filename+": origin: "+headersRead+"; set: "+headers.join("|");
                //TODO: rozrzerzenie tab o nowe headers
            }
            file.close();
        }else {
            qDebug() << "Error opened "+filename+": "+file.errorString();
        }

    }

}

bool TabDB::insert(QStringList record)
{
    if(record.count() != headers.count()) {
        qDebug() << "Error of items in the record: "+record.join("|")+" != "+headers.join("|");
        return false;
    }
    if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << record.join("|")+"\n";
        qDebug() << "Insert new record to "+filename+": "+record.join("|");
        file.close();
        return true;
    }else {
        qDebug() << "Error opened "+filename+": "+file.errorString();
        return false;
    }

}

bool TabDB::insert(QList<QStringList> records)
{
    if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        foreach (QStringList record, records) {
            if(record.count() != headers.count()) {
                qDebug() << "Error of items in the record: "+record.join("|")+" != "+headers.join("|");
                return false;
            }else {
                out << record.join("|")+"\n";
                qDebug() << "Insert new record to "+filename+": "+record.join("|");
            }
        }
        file.close();
        return true;
    }else {
        qDebug() << "Error opened "+filename+": "+file.errorString();
        return false;
    }
}

bool TabDB::remove(QString id)
{
    QStringList lines;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int l=1;
        lines << in.readLine(); // read headers;
        while(!in.atEnd()) {
            QString line = in.readLine();
            l++;
            QStringList record = line.split("|");
            if(!record.isEmpty()) {
                if(record.first() != id) {
                    lines << line;
                }else {
                    qDebug() << "Removed the record: "+line;
                }
            }else {
                qDebug() << "Empty record in line "+QString::number(l)+" in "+filename; // the last line be empty
            }
        }
        file.close();
        if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            foreach (QString line, lines) {
                out << line+"\n";
            }
            file.close();
            qDebug() << "Saved tab "+filename+" with "+QString::number(lines.count()-1)+" records";
            return true;
        }else {
            qDebug() << "Error opened "+filename+": "+file.errorString();
            return false;
        }
    }else {
        qDebug() << "Error opened "+filename+": "+file.errorString();
        return false;
    }
}

bool TabDB::remove(int where, QString value)
{
    QStringList lines;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int l=1;
        lines << in.readLine(); // read headers;
        while(!in.atEnd()) {
            QString line = in.readLine();
            l++;
            QStringList record = line.split("|");
            if(!record.isEmpty()) {
                if(record.count()>where) {
                    if(record.at(where) != value) {
                        lines << line;
                    }else {
                        qDebug() << "Removed the record: "+line;
                    }
                }else {
                    qDebug() << "Wrong format of record in line"+QString::number(l)+" in "+filename;
                }
            }else {
                qDebug() << "Empty record in line "+QString::number(l)+" in "+filename; // the last line be empty
            }
        }
        file.close();
        if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            foreach (QString line, lines) {
                out << line+"\n";
            }
            file.close();
            qDebug() << "Saved tab "+filename+" with "+QString::number(lines.count()-1)+" records";
            return true;
        }else {
            qDebug() << "Error opened "+filename+": "+file.errorString();
            return false;
        }
    }else {
        qDebug() << "Error opened "+filename+": "+file.errorString();
        return false;
    }
}

bool TabDB::update(QString id, QStringList updateRecord)
{
    if(updateRecord.count() != headers.count()) {
        qDebug() << "Error of items in the record: "+updateRecord.join("|")+" != "+headers.join("|");
        return false;
    }

    QStringList lines;
    bool updated = false;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int l=1;
        lines << in.readLine(); // read headers;
        while(!in.atEnd()) {
            QString line = in.readLine();
            l++;
            QStringList record = line.split("|");
            if(!record.isEmpty()) {
                if(record.first() != id) {
                    lines << line;
                }else {
                    updated = true;
                    lines << updateRecord.join("|");
                    qDebug() << "Removed the record: "+line;
                }
            }else {
                qDebug() << "Empty record in line "+QString::number(l)+" in "+filename; // the last line be empty
            }
        }
        file.close();
        if(!updated)
            lines << updateRecord.join("|");

        if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            foreach (QString line, lines) {
                out << line+"\n";
            }
            file.close();
            qDebug() << "Saved tab "+filename+" with "+QString::number(lines.count()-1)+" records";
            return true;
        }else {
            qDebug() << "Error opened "+filename+": "+file.errorString();
            return false;
        }
    }else {
        qDebug() << "Error opened "+filename+": "+file.errorString();
        return false;
    }
}

bool TabDB::select(QList<QStringList> *records)
{
    records->clear();
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int l=1;
        in.readLine(); // read headers;
        while(!in.atEnd()) {
            QStringList record = in.readLine().split("|");
            l++;
            if(record.count() != headers.count()) {
                qDebug() << "Error of items in the record in line "+QString::number(l)+": "+record.join("|")+" != "+headers.join("|");
                return false;
            }else {
                *records << record;
            }
        }
        file.close();
        qDebug() << "Selected "+QString::number(records->count())+" records from "+filename;
        return true;
    }else {
        qDebug() << "Error opened "+filename+": "+file.errorString();
        return false;
    }
}

bool TabDB::select(QString search, QList<QStringList> *records)
{
    records->clear();
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int l=1;
        in.readLine(); // read headers;
        while(!in.atEnd()) {
            QStringList record = in.readLine().split("|");
            l++;
            if(record.count() != headers.count()) {
                qDebug() << "Error of items in the record in line "+QString::number(l)+": "+record.join("|")+" != "+headers.join("|");
                return false;
            }else {
                bool found = false;
                foreach(QString val, record) {
                    if(val.contains(search,Qt::CaseInsensitive))
                        found = true;

                }
                if(found)
                    *records << record;

            }
        }
        file.close();
        qDebug() << "Selected "+QString::number(records->count())+" records from "+filename;
        return true;
    }else {
        qDebug() << "Error opened "+filename+": "+file.errorString();
        return false;
    }
}

bool TabDB::select(int header, QStringList *items)
{
    items->clear();
    if(header < 0 || header >= headers.count()) {
        qDebug() << "Error number of header: 0 <= '"+QString::number(header)+"' < "+QString::number(headers.count());
        return false;
    }
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int l=1;
        in.readLine(); // read headers;
        while(!in.atEnd()) {
            QStringList record = in.readLine().split("|");
            l++;
            if(record.count() != headers.count()) {
                qDebug() << "Error of items in the record in line "+QString::number(l)+": "+record.join("|")+" != "+headers.join("|");
                return false;
            }else {
                *items << record.at(header);
            }
        }
        file.close();
        qDebug() << "Selected "+QString::number(items->count())+" items from "+filename;
        return true;
    }else {
        qDebug() << "Error opened "+filename+": "+file.errorString();
        return false;
    }
}

bool TabDB::select(int header, int where, QString value, QStringList *items)
{
    items->clear();
    if(header < 0 || header >= headers.count()) {
        qDebug() << "Error number of header: 0 <= '"+QString::number(header)+"' < "+QString::number(headers.count());
        return false;
    }
    if(where < 0 || where >= headers.count()) {
        qDebug() << "Error number of where(header): 0 <= '"+QString::number(where)+"' < "+QString::number(headers.count());
        return false;
    }
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int l=1;
        in.readLine(); // read headers;
        while(!in.atEnd()) {
            QStringList record = in.readLine().split("|");
            l++;
            if(record.count() != headers.count()) {
                qDebug() << "Error of items in the record in line "+QString::number(l)+": "+record.join("|")+" != "+headers.join("|");
                return false;
            }else {
                if(record.at(where) == value || record.at(where).isEmpty()) {
                    *items << record.at(header);
                }
            }
        }
        file.close();
        qDebug() << "Selected "+QString::number(items->count())+" items from "+filename;
        return true;
    }else {
        qDebug() << "Error opened "+filename+": "+file.errorString();
        return false;
    }
}

bool TabDB::select(int where, QString value, QList<QStringList> *rows)
{
    rows->clear();
    if(where < 0 || where >= headers.count()) {
        qDebug() << "Error number of where(header): 0 <= '"+QString::number(where)+"' < "+QString::number(headers.count());
        return false;
    }
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int l=1;
        in.readLine(); // read headers;
        while(!in.atEnd()) {
            QStringList record = in.readLine().split("|");
            l++;
            if(record.count() != headers.count()) {
                qDebug() << "Error of items in the record in line "+QString::number(l)+": "+record.join("|")+" != "+headers.join("|");
                return false;
            }else {
                if(record.at(where) == value) {
                    *rows << record;
                }
            }
        }
        file.close();
        qDebug() << "Selected "+QString::number(rows->count())+" rows from "+filename;
        return true;
    }else {
        qDebug() << "Error opened "+filename+": "+file.errorString();
        return false;
    }
}

bool TabDB::count(int *no)
{
    *no = 0;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.readLine(); // read headers;
        int l = 1;
        while(!in.atEnd()) {
            QString line = in.readLine();
            if(!line.isEmpty()) {
                QStringList record = line.split("|");
                l++;

                if(record.count() != headers.count()) {
                    qDebug() << "Error of items in the record in line "+QString::number(l)+": "+record.join("|")+" != "+headers.join("|");
                    *no = -1;
                    return false;
                }else {
                    *no = *no + 1;
                }
            }

        }
        file.close();
        qDebug() << "Counted "+QString::number(*no)+" records from "+filename;
        return true;
    }else {
        qDebug() << "Error opened "+filename+": "+file.errorString();
        *no = -1;
        return false;
    }
}

bool TabDB::lastID(int *id)
{
    *id = 0;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.readLine(); // read headers;
        int l = 1;
        while(!in.atEnd()) {
            QString line = in.readLine();
            if(!line.isEmpty()) {
                QStringList record = line.split("|");
                l++;

                if(record.count() != headers.count()) {
                    qDebug() << "Error of items in the record in line "+QString::number(l)+": "+record.join("|")+" != "+headers.join("|");
                    *id = -1;
                    return false;
                }else {
                    *id = record.first().toInt();
                }
            }

        }
        file.close();
        qDebug() << "Last ID: "+QString::number(*id)+" from "+filename;
        return true;
    }else {
        qDebug() << "Error opened "+filename+": "+file.errorString();
        *id = -1;
        return false;
    }
}

QStringList TabDB::getHeaders()
{
    return headers;
}

QString TabDB::getFilename()
{
    return filename;
}
