#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QTcpServer>
#include <QVector>
class QThread;
class ServerWorker;
class ChatServer : public QTcpServer
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatServer)
public:
    explicit ChatServer(QObject* parent = nullptr);
protected:
    void incomingConnection(qintptr socketDescriptor) override;
private:
    QVector<ServerWorker*> m_clients;
private slots:
    void broadcast(const QByteArray& message,ServerWorker* exclude);
    void jsonReceived(ServerWorker* sender,const QJsonDocument& doc);
    void userDisconnected(ServerWorker* sender);
    void userError(ServerWorker* sender);
private:
    void jsonFromLoggedOut(ServerWorker *sender, const QJsonDocument &doc);
    void jsonFromLoggedIn(ServerWorker *sender, const QJsonDocument &doc);
    void sendJson(ServerWorker *destination, const QByteArray& message);
};

#endif // CHATSERVER_H
