#include "chatserver.h"
#include "chatsession.h"

#include <functional>

#include <QDebug>

ChatServer::ChatServer(QObject * parent)
    : QTcpServer(parent), listenPort(0)
{
    QObject::connect(this, &QTcpServer::newConnection, this, &ChatServer::openSessions);
}

ChatServer::ChatServer(quint16 port, QObject * parent)
    : ChatServer(parent)
{
    setPort(port);
}

quint16 ChatServer::port() const
{
    return listenPort;
}

void ChatServer::setPort(quint16 port)
{
    listenPort = port;
}

void ChatServer::start()
{
    // Do nothing if we're already started
    if (isListening())
        return;

    // Start listening for new connections
    if (!listen(QHostAddress::Any, listenPort))  {
        emit statusReport(QStringLiteral("Server failed to start!"));
        return;
    }

    emit started();
    emit statusReport(QStringLiteral("Server started"));
}

void ChatServer::stop()
{
    // Do nothing if we aren't listening
    if (!isListening())
        return;

    // Stop listening
    emit aboutToStop();

    close();

    emit stopped();
    emit statusReport(QStringLiteral("Server stopped"));
}

void ChatServer::toggle()
{
    if (isListening())
        stop();
    else
        start();
}


void ChatServer::openSessions()
{
    while (hasPendingConnections()) {
        // Create a user session for each pending connection
        ChatSession * session = new ChatSession(this);
        QTcpSocket * socket = nextPendingConnection();
        if (!session->open(socket))  {
            emit statusReport(QStringLiteral("Couldn't initialize client session."));
            continue;
        }

        // Log the received messages & status (delegate the signals)
        QObject::connect(session, &ChatSession::statusReport, this, &ChatServer::statusReport);
        QObject::connect(session, &ChatSession::received, this, &ChatServer::messageReceived);

        // Take care of the cleaning up
        QObject::connect(this, &ChatServer::aboutToStop, session, &ChatSession::close);
        QObject::connect(session, &ChatSession::closed, this, std::bind(&ChatServer::closeSession, this, session));
        QObject::connect(session, &ChatSession::closed, session, &ChatSession::deleteLater);

        // Connect all the existing clients to the new one
        for (ChatSessionList::ConstIterator i = participants.constBegin(), end = participants.constEnd(); i != end; ++i)  {
            QObject::connect(*i, &ChatSession::received, session, QOverload<const ChatMessagePointer &>::of(&ChatSession::send));
            QObject::connect(session, &ChatSession::received, *i, QOverload<const ChatMessagePointer &>::of(&ChatSession::send));
        }

        // Append the new session to our participants list
        participants.append(session);

        emit statusReport(QStringLiteral("A client has connected"));
    }
}

void ChatServer::closeSession(ChatSession * session)
{
    // Just remove the closing session from the participants list
    participants.removeOne(session);

    emit statusReport(QStringLiteral("A client has disconnected"));
}
