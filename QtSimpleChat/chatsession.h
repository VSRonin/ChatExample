#ifndef CHATSESSION_H
#define CHATSESSION_H

#include <QObject>
#include <QByteArray>

class QTcpSocket;
class QJsonObject;

class ChatSessionPrivate;
class ChatSession : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatSession)

public:
    explicit ChatSession(QObject * parent = nullptr);

    bool open(qintptr);
    bool open(QTcpSocket *);
    void close();

public slots:
    void send(const QJsonObject &);

signals:
    void opened();
    void closed();
    void error();
    void received(const QJsonObject &);

private slots:
    void initialize();
    void readData();

private:
    QTcpSocket * socket;
};

#endif // CHATSESSION_H
