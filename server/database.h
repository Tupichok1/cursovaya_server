#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>
#include <QProcess>
#include <QDebug>
#include <iostream>
#include <QTcpSocket>

class dataBase
{
private:
    static dataBase* DB;
    QSqlDatabase db;
    dataBase();
    QTcpSocket* socket;

public:
    static dataBase* getInstance();
    void closeDB();
    void dumpDB();
    void recoverDB(const QString& file);
    QSqlQuery* takeQuery();
};

#endif // DATABASE_H
