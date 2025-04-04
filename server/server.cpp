#include "server.h"
#include <QHostAddress>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <sstream>
#include <QDateTime>
#include <QProcess>

Server::Server() {

    server = new QTcpServer;
    connect(server, &QTcpServer::newConnection, this, &Server::slotNewConnection);
    server -> listen(QHostAddress::Any, 1234);

    db = dataBase::getInstance();
    query = db -> takeQuery();
}

void Server::slotNewConnection() {

    socket = server -> nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotServerRead);
}

void Server::slotServerRead() {

    std::string command;
    std::string message = QString::fromUtf8(socket -> readAll()).toStdString();
    std::stringstream sstream(message);

    qDebug() << QString::fromStdString(message);

    while ( getline(sstream, command, ' ') ) {

        if ( command == "check_user" ) {
            sstream >> command;
            checkUser(QString::fromStdString(command));
        }

        if ( command == "user_deposit" ) {
            sstream >> command;
            userDeposit(QString::fromStdString(command));
        }

        if ( command == "change_deposit" ) {
            std::string change, orderID;
            sstream >> change;
            sstream >> orderID;
            closeDeposit(QString::fromStdString(change), QString::fromStdString(orderID));
        }

        if ( command == "insert_orders" ) {
            std::string deposit_type, exchange;
            sstream >> deposit_type;
            sstream >> exchange;
            insertOrder(QString::fromStdString(deposit_type), QString::fromStdString(exchange));
        }

        if ( command == "insert_bank" ) {
            std::string userID, order_id, amount, date;
            sstream >> userID;
            sstream >> order_id;
            sstream >> amount;
            sstream >> date;
            insertBank(QString::fromStdString(order_id), QString::fromStdString(userID), QString::fromStdString(amount), QString::fromStdString(date));
        }

        if ( command == "reg_acc" ) {
            std::string firstName, secondName, login, password;
            sstream >> firstName;
            sstream >> secondName;
            sstream >> login;
            sstream >> password;
            regAccount(QString::fromStdString(firstName), QString::fromStdString(secondName), QString::fromStdString(login), QString::fromStdString(password));
        }

        if ( command == "check_orders" ) {
            checkOrder();
        }

        if ( command == "take_dumpes" ) {
            takeDump();
        }

        if ( command == "make_dump" ) {
            makeDump();
            break;
        }

        if ( command == "make_recover" ) {
            sstream >> command;
            recoverDB(command);
        }

        if ( command == "sql_command" ) {

            QString command_;
            std::string word;
            while ( getline(sstream, word, ' ') ) {

                command_ += QString::fromStdString(word) + " ";
            }

            sqlCommand(command_);
        }
    }
}

void Server::checkUser(const QString& login) {
    query -> exec("SELECT user_id, password, root FROM users WHERE login = '" + login + "'");

    if ( query -> size() <= 0 ) {
        socket -> write(QString("false").toUtf8());
        return;
    }

    query -> first();

    QString message = query -> value(0).toString() + "_" + query -> value(1).toString() + "_" + query -> value(2).toString();
    socket -> write(QString(message).toUtf8());
}

void Server::userDeposit(const QString& userID) {

    QString response;

    query -> exec("select b.amount, d.percent, d.duration_month, e.exchange_name, b.date, b.order_id "
                "from orders o, bank b, deposit_type d, exchanges e "
                "WHERE o.order_id = b.order_id AND o.deposit_type_id = d.deposit_type_id "
                "AND o.exchange_id = e.exchange_id AND b.is_allowed = 'approved' AND b.user_id = '" + userID + "'");

    if ( query -> size() <= 0 ) {
        socket -> write("false");
        return;
    }

    while ( query -> next() ) {

        for ( int i = 0; i < 6; i ++ ) {

            if ( i != 5 ) {
                response += query -> value(i).toString() + "_";
            }

            else {
                response += query -> value(i).toString();
            }
        }

        response += "|";
    }

    socket -> write(response.toUtf8());
}

void Server::closeDeposit(const QString& change, const QString& orderID) {

    query -> exec("update bank set is_allowed = '" + change + "' WHERE order_id = '" + orderID + "'");

    socket -> write(QString("Succesfully").toUtf8());
}

void Server::insertOrder(const QString& deposit_type, const QString& exchange) {

    query -> exec("INSERT INTO orders (deposit_type_id, exchange_id) VALUES ('" + deposit_type + "', '" + exchange + "') RETURNING order_id");

    query -> first();
    socket -> write(query -> value(0).toString().toUtf8());
}

void Server::insertBank(const QString& order_id, const QString& userID, const QString& amount, const QString& date) {

    query -> exec("INSERT INTO bank VALUES ('" + userID + "', '" + order_id + "', '" + amount + "', '" + date + "', 'consideration')");
    socket -> write("Succeed");
}

void Server::regAccount(const QString& firstName, const QString& secondName, const QString& login, const QString& password) {

    query -> exec("INSERT INTO users (firstname, secondname, login, password, root) VALUES ('"
                + firstName + "', '" + secondName + "', '" + login + "', '" + password + "', 'user')");
    socket -> write("Succesfully");
}

void Server::recoverDB(const std::string& file) {

    QProcess* process = new QProcess();

    process -> setStandardInputFile("/home/tupichok/db_reserve/" + QString::fromStdString(file));
    process -> start("pg_restore", QStringList() << "-c" << "-U" << "postgres" << "-h" << "localhost" << "-p" << "5432" << "-d" << "pyramid");
}

void Server::checkOrder() {

    QString response;

    query -> exec("SELECT o.order_id, b.amount, d.percent, d.duration_month, e.exchange_name, b.date "
                "FROM orders o, bank b, deposit_type d, exchanges e "
                "WHERE o.order_id = b.order_id AND o.deposit_type_id = d.deposit_type_id "
                "AND o.exchange_id = e.exchange_id AND b.is_allowed = 'consideration'");

    while ( query -> next() ) {

        for ( int i = 0; i < 6; i ++ ) {

            if ( i != 5 ) {
                response += query -> value(i).toString() + "_";
            }

            else {
                response += query -> value(i).toString();
            }
        }

        response += "|";
    }

    socket -> write(response.toUtf8());
}

void Server::makeDump() {

    QString current = QDateTime::currentDateTime().toString("dd.MM.yyyy_hh:mm:ss");

    QProcess* process = new QProcess();
    QStringList params;

    process -> setStandardOutputFile("/home/tupichok/db_reserve/" + current + ".dump");
    process -> start("pg_dump",  QStringList() << "-Fc" << "-U" << "postgres" << "-h" << "localhost" << "-p" << "5432" << "-d" << "pyramid");

    socket -> write("Succesfully");
}

void Server::takeDump() {

    QString message;
    QString path = "/home/tupichok/db_reserve/";

    QDir dir(path);
    QFileInfoList fileList = dir.entryInfoList();

    for ( int i = 0; i < fileList.size(); i ++ ) {

        if ( !fileList.at(i).isFile() ) {
            continue;
        }

        message += fileList.at(i).fileName() + " ";
    }

    socket -> write(message.toUtf8());
}

void Server::sqlCommand(const QString& command_) {

    if ( command_ == "" ) {
        socket -> write("empty line");
        return;
    }

    QProcess* process = new QProcess();

    process -> start("psql", QStringList() << "-U" << "postgres" << "-h" << "localhost" << "-p" << "5432" << "-d" << "pyramid" << "-c" << command_);
    process -> waitForFinished();

    QString output = QString::fromUtf8(process -> readAllStandardError());

    if ( output != "" ) {
        socket -> write(output.toUtf8());
        return;
    }

    socket -> write(process -> readAllStandardOutput());
}













