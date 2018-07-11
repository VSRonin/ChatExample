#ifndef CHATSERVER_H
#define CHATSERVER_H

#include "qtsimplechat.h"

#include <QTcpServer>
#include <QHash>

class ChatSession;
class ChatServer : public QTcpServer
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatServer)

    typedef QHash<QString, ChatSession *> ChatSessionHash;

public:
    explicit ChatServer(quint16 = 0, QObject * parent = nullptr);

    quint16 port() const;
    void setPort(quint16);

public slots:
    void start();
    void stop();
    void toggle(); // Start/stop

signals:
    void started();
    void aboutToStop();
    void stopped();

    void statusReport(const QString &);
    void messageReceived(const ChatMessagePointer &);

private slots:
    void openSessions();
    void closeSession(const QString &);
    void processMessage(ChatSession *, const ChatMessagePointer &);

private:
    quint16 listenPort;
    ChatSessionHash participants;
};

#endif // CHATSERVER_H
