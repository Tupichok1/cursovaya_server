#include "sql.h"

QString noCommand::perform() {

    return "";
}

where::where(const std::vector<QString>& columnName, const std::vector<QString>& conditions, const std::vector<QString>& operations) {

    this -> columnName = columnName;
    this -> conditions = conditions;
    this -> operations = operations;
}

QString where::perfom() {

    QString command = " WHERE ";

    for ( size_t i = 0; i < columnName.size(); i ++ ) {

        if ( i != operations.size() ) {
            command = command + columnName[i] + " = " + conditions[i] + " " + opeartions[i];
        }

        else {
            command = command + columnName[i] + " = " + conditions[i];
        }
    }

    return command;
}

returning::returning(const QString& valueForReturn) {

    this -> valueForReturn = valueForReturn;
}

QString returning::perform() {

    return " RETURNING " + valueForReturn;
}

sql::sql() {
    db = dataBase::getInstance();
    filter_ = new noCommand();
    QSqlQuery = db -> takeQuery();
}

void sql::setFilter(filter* filter_) {

    this -> filter_ = filter_;
}

QSqlQuery* insert::function(const QString& tableName, const std::vector<QString>& columnNames, const std::vector<QString>& values) {


}










































