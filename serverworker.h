#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTcpSocket>
class ServerWorker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ServerWorker)
public:
    explicit ServerWorker(QObject* parent = nullptr);
    QTcpSocket* socket() const;
    virtual bool setSocketDescriptor(qintptr socketDescriptor, QTcpSocket::SocketState socketState = QTcpSocket::ConnectedState, QIODevice::OpenMode openMode = QIODevice::ReadWrite);
    QString userName() const;
    void setUserName(const QString &userName);
    void sendJson(const QByteArray& jsonData);
private slots:
    void receiveJson();
signals:
    void jsonReceived(const QJsonDocument& jsonDoc);
private:
    QTcpSocket* m_serverSocket;
    QString m_userName;
};

#endif // SERVERWORKER_H
