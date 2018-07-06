#ifndef CHATSERVER_H
#define CHATSERVER_H

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

private slots:
    void openSessions();
    void closeSession(ChatSession *);

private:
    ChatSessionList participants;
};

#endif // CHATSERVER_H
