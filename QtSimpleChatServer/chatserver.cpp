#include "chatserver.h"
#include "chatsession.h"
#include "chatmessage.h"

#include <functional>

#include <QDebug>

ChatServer::ChatServer(quint16 port, QObject * parent)
    : QTcpServer(parent), listenPort(port)
{
    QObject::connect(this, &QTcpServer::newConnection, this, &ChatServer::openSessions);
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
        QObject::connect(session, &ChatSession::received, this, std::bind(&ChatServer::processMessage, this, session, std::placeholders::_1));

        // Take care of the cleaning up
        QObject::connect(this, &ChatServer::aboutToStop, session, &ChatSession::close);
        QObject::connect(session, &ChatSession::closed, session, &ChatSession::deleteLater);

        emit statusReport(QStringLiteral("A client has connected"));
    }
}

void ChatServer::closeSession(const QString & username)
{
    // Just remove the closing session from the participants list
    if (!participants.remove(username))
        return;

    emit statusReport(QStringLiteral("A client has disconnected"));

    // Notify the other participants a client has quit
    ChatMessagePointer logoutMessage(new ChatMessageLogout());
    logoutMessage->setUsername(username);
    broadcast(logoutMessage);
}

void ChatServer::processMessage(ChatSession * session, const ChatMessagePointer & message)
{
    switch (message->type())
    {
    case ChatMessage::LoginType:
        {
            const ChatMessageLogin * loginMessage = reinterpret_cast<const ChatMessageLogin *>(message.data());

            QString username = loginMessage->username();

            ChatMessageLoginStatus * statusMessage = new ChatMessageLoginStatus();
            statusMessage->setUsername(username);

            if (participants.contains(username))  {
                statusMessage->setStatus(ChatMessageLoginStatus::Fail);
                statusMessage->setErrorText(QStringLiteral("Username already in use"));
            }
            else  {
                statusMessage->setStatus(ChatMessageLoginStatus::Success);

                // Unregister the client if the connection is closed prematurely
                QObject::connect(session, &ChatSession::closed, this, std::bind(&ChatServer::closeSession, this, username));
                // Connect all the existing clients to the new one
                for (ChatSessionHash::ConstIterator i = participants.constBegin(), end = participants.constEnd(); i != end; ++i)  {
                    QObject::connect(*i, &ChatSession::received, session, QOverload<const ChatMessagePointer &>::of(&ChatSession::send));
                    QObject::connect(session, &ChatSession::received, *i, QOverload<const ChatMessagePointer &>::of(&ChatSession::send));
                }

                // Notify the other participants we have a new client connected
                ChatMessagePointer loginMessage(new ChatMessageLogin());
                loginMessage->setUsername(username);
                broadcast(loginMessage);

                // Insert the session into the active participants
                participants.insert(username, session);
            }

            send(session, ChatMessagePointer(statusMessage));
        }
        break;
    case ChatMessage::LogoutType:
        {
            const ChatMessageLogout * logoutMessage = reinterpret_cast<const ChatMessageLogout *>(message.data());

            QString username = logoutMessage->username();
            ChatSession * session = participants.value(username, nullptr);

            Q_ASSERT(session);
            QMetaObject::invokeMethod(session, "close", Qt::QueuedConnection);
        }
        break;
    default:
        ;
    }
}

inline void ChatServer::broadcast(const ChatMessagePointer & message)
{
    for (ChatSessionHash::ConstIterator i = participants.constBegin(), end = participants.constEnd(); i != end; ++i)
        send(i.value(), message);
}

inline void ChatServer::send(ChatSession * session, const ChatMessagePointer & message)
{
    QMetaObject::invokeMethod(session, "send", Qt::QueuedConnection, Q_ARG(ChatMessagePointer, message));
}


