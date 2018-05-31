#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QTcpSocket>
class QHostAddress;
class QJsonDocument;
class ChatClient : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatClient)
public:
    explicit ChatClient(QObject *parent = nullptr);
public slots:
    void connectToServer(const QHostAddress &address, quint16 port);
    void login(const QString& userName);
    void sendMessage(const QString& text);
    void disconnectFromHost();
signals:
    void connected();
    void loggedIn();
    void loginError(const QString& reason);
    void disconnected();
    void messageReceived(const QString& sender, const QString& text);
    void error(QAbstractSocket::SocketError socketError);
    void userJoined(const QString& username);
    void userLeft(const QString& username);
private:
    QTcpSocket* m_clientSocket;
    bool m_loggedIn;
    void jsonReceived(const QJsonObject& doc);
private slots:
    void onReadyRead();
};

#endif // CHATCLIENT_H
