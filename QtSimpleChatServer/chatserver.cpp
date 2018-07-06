#include "chatserver.h"
#include "chatsession.h"

#include <functional>

#include <QDebug>

ChatServer::ChatServer(QObject * parent)
    : QTcpServer(parent)
{
    QObject::connect(this, &QTcpServer::newConnection, this, &ChatServer::openSessions);
}

void ChatServer::openSessions()
{
    while (hasPendingConnections()) {
        // Create a user session for each pending connection
        ChatSession * session = new ChatSession(this);
        QTcpSocket * socket = nextPendingConnection();
        if (!session->open(socket))  {
            qDebug() << "Couldn't initialize client session";
            continue;
        }

        // Take care of the cleaning up
        QObject::connect(session, &ChatSession::closed, this, std::bind(&ChatServer::closeSession, this, session));
        QObject::connect(session, &ChatSession::closed, session, &ChatSession::deleteLater);

        // Connect all the existing clients to the new one
        for (ChatSessionList::ConstIterator i = participants.constBegin(), end = participants.constEnd(); i != end; ++i)  {
            QObject::connect(*i, &ChatSession::received, session, QOverload<const ChatMessagePointer &>::of(&ChatSession::send));
            QObject::connect(session, &ChatSession::received, *i, QOverload<const ChatMessagePointer &>::of(&ChatSession::send));
        }

        // Append the new session to our participants list
        participants.append(session);

        qDebug() << "A client has connected";
    }
}

void ChatServer::closeSession(ChatSession * session)
{
    // Just remove the closing session from the participants list
    participants.removeOne(session);

    qDebug() << "A client has disconnected";
}
