#ifndef CHATSERVER_H
#define CHATSERVER_H

#include "qtsimplechat.h"

#include <QTcpServer>
#include <QList>

class ChatSession;
class ChatServer : public QTcpServer
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatServer)

    typedef QList<ChatSession *> ChatSessionList;

public:
    explicit ChatServer(QObject * parent = nullptr);
    explicit ChatServer(quint16, QObject * parent = nullptr);

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
    void closeSession(ChatSession *);

private:
    quint16 listenPort;
    ChatSessionList participants;
};

#endif // CHATSERVER_H
