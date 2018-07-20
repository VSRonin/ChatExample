#include "chatserverthreaded.h"
#include "chatsession.h"

ChatServerThreaded::ChatServerThreaded(quint16 port, QObject * parent)
    : ChatServer(port, parent)
{
    qRegisterMetaType<qintptr>("qintptr");
}

void ChatServerThreaded::incomingConnection(qintptr handle)
{
    // Create a user session for the pending connection
    ChatSession * session = createSession();

    // Move the session to the correct thread
    session->setParent(nullptr);
    threadPool.moveObject(session);

    // Try to open the session in the correct thread
    bool opened;
    QMetaObject::invokeMethod(session, "open", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, opened), Q_ARG(qintptr, handle));

    if (!opened)  {
        emit statusReport(QStringLiteral("Couldn't initialize client session."));
        return;
    }

    emit statusReport(QStringLiteral("A client has connected"));
}
