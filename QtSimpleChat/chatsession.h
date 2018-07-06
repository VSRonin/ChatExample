#ifndef CHATSESSION_H
#define CHATSESSION_H

#include "qtsimplechat.h"
#include <QObject>

class QTcpSocket;
class QJsonObject;

class ChatSessionPrivate;
class QTSIMPLECHAT_EXPORT ChatSession : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatSession)

public:
    explicit ChatSession(QObject * parent = nullptr);

public slots:
    bool open(qintptr);
    bool open(QTcpSocket *);
    void close();
    void send(const ChatMessage &);
    void send(const ChatMessagePointer &);

signals:
    void opened();
    void closed();
    void error();
    void received(const ChatMessagePointer &);
    void statusReport(const QString &);

private slots:
    void readData();

private:
    void initialize();
    void decodeJson(const QJsonObject &);

    QTcpSocket * socket;
};

#endif // CHATSESSION_H
