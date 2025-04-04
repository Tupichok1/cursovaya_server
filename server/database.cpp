#include "database.h"
#include <QObject>

dataBase* dataBase::DB = NULL;

dataBase* dataBase::getInstance() {

    if ( DB == NULL ) {
        DB = new dataBase();
    }

    return DB;
}

dataBase::dataBase() {

    db = QSqlDatabase::addDatabase("QPSQL", "cursovaya");
    db.setHostName("db_auth");
    db.setPort(5432);
    db.setDatabaseName("pyramid");
    db.setUserName("postgres");
    db.setPassword("root");
    db.open();
}

QSqlQuery* dataBase::takeQuery() {

    return new QSqlQuery(db);
}

void dataBase::closeDB() {

    db.close();
    QSqlDatabase::removeDatabase("cursovaya");
}

void dataBase::dumpDB() {

    socket = new QTcpSocket;
    socket -> connectToHost("localhost", 1234);
    socket -> write(QString("makeDump").toUtf8());
}

void dataBase::recoverDB(const QString& file) {

    socket = new QTcpSocket;
    socket -> connectToHost("localhost", 1234);
    socket -> write(QString("recover" + file).toUtf8());
}
