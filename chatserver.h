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
    const int m_idealThreadCount;
    QVector<QThread*> m_availableThreads;
    QVector<int> m_threadsLoad;
    QVector<ServerWorker*> m_clients;
private slots:
    void broadcast(const QByteArray& message,ServerWorker* exclude);
    void jsonReceived(ServerWorker* sender,const QJsonDocument& doc);
    void userDisconnected(ServerWorker* sender,int threadIdx);
private:
    void jsonFromLoggedOut(ServerWorker *sender, const QJsonDocument &doc);
    void jsonFromLoggedIn(ServerWorker *sender, const QJsonDocument &doc);
};

#endif // CHATSERVER_H
