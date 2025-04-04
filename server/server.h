#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QVector>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlQuery>
#include "database.h"

class Server : public QMainWindow
{
    Q_OBJECT

public:
    void slotNewConnection();
    void slotServerRead();
    Server();

private:
    QTcpServer* server;
    QTcpSocket* socket;
    QSqlQuery* query;
    dataBase* db;
    void SendToClient();
    void takeDump();
    void makeDump();
    void checkUser(const QString& login);
    void userDeposit(const QString& userID);
    void closeDeposit(const QString& change, const QString& orderID);
    void insertOrder(const QString& deposit_type, const QString& exchange);
    void insertBank(const QString& order_id, const QString& userID, const QString& amount, const QString& date);
    void regAccount(const QString& firstName, const QString& secondName, const QString& login, const QString& password);
    void checkOrder();
    void recoverDB(const std::string& file);
    void sqlCommand(const QString& command_);
};
#endif // SERVER_H
