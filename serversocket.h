#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <QObject>
#include <QTcpSocket>
class ServerWorker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ServerWorker)
public:
    explicit ServerWorker(QObject* parent = nullptr);
    virtual bool setSocketDescriptor(qintptr socketDescriptor, QTcpSocket::SocketState socketState = ConnectedState, QTcpSocket::OpenMode openMode = ReadWrite);
private slots:
    void receiveJson();
    void sendJson(const QByteArray& jsonData);
signals:
    void jsonReceived(const QJsonDocument& jsonDoc);
private:
    QTcpSocket* m_serverSocket;
};

#endif // SERVERSOCKET_H
