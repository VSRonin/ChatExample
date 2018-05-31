#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QTcpServer>
#include <QVector>
class QThread;
class ServerWorker;
class QJsonObject;
class ChatServer : public QTcpServer
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatServer)
public:
    explicit ChatServer(QObject* parent = nullptr);
    ~ChatServer();
protected:
    void incomingConnection(qintptr socketDescriptor) override;
private:
    const int m_idealThreadCount;
    QVector<QThread*> m_availableThreads;
    QVector<int> m_threadsLoad;
    QVector<ServerWorker*> m_clients;
private slots:
    void broadcast(const QJsonObject& message,ServerWorker* exclude);
    void jsonReceived(ServerWorker* sender,const QJsonObject& doc);
    void userDisconnected(ServerWorker* sender,int threadIdx);
    void userError(ServerWorker* sender);
private:
    void jsonFromLoggedOut(ServerWorker *sender, const QJsonObject &doc);
    void jsonFromLoggedIn(ServerWorker *sender, const QJsonObject &doc);
    void sendJson(ServerWorker *destination, const QJsonObject &message);
};

#endif // CHATSERVER_H
