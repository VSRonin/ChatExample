#ifndef CHATSESSION_H
#define CHATSESSION_H

#include <QObject>

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

signals:
    void opened();
    void closed();
    void error();
    void received(const QJsonObject &);

public slots:
    void send(const QJsonObject &);

private slots:
    void readData();

private:
    void initialize();

    QTcpSocket * socket;
};

#endif // CHATSESSION_H
