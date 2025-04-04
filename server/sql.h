#ifndef SQL_H
#define SQL_H

#include "database.h"
#include <QSqlQuery>
#include <vector>
#include <QDebug>

class filter {
public:

    virtual QString perform() = 0;
};

class noCommand : public filter {
public:
    QString perform() override;
};

class where : public filter {
private:
    std::vector<QString> columnName;
    std::vector<QString> conditions;
    std::vector<QString> operations;

public:
    where(const std::vector<QString>& columnName, const std::vector<QString>& conditions, const std::vector<QString>& operations);
    QString perform() override;
};

class returning : public filter {
private:
    QString valueForReturn;

public:
    returning(const QString& valueForReturn);
    QString perform() override;
};

class sql
{

protected:
    dataBase* db;
    filter* filter_;
    QSqlQuery* query;

public:
    sql();
    void setFilter(filter* filter_);
};

class insert : public sql {
public:
    QSqlQuery* function(const QString& tableName, const std::vector<QString>& columnNames, const std::vector<QString>& values);
};

class Select : public sql {
public:
    QSqlQuery* function(const std::vector<QString>& tableNames, const std::vector<QString>& columnNames);
};

class Update : public sql {
public:
    void function(const QString& tableName, const std::vector<QString>& columnNames, const std::vector<QString>& conditions);
};

#endif // SQL_H
