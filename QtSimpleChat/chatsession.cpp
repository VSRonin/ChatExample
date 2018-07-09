#include "chatsession.h"
#include "chatmessage.h"

#include <QDataStream>
#include <QTcpSocket>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

ChatSession::ChatSession(QObject * parent)
    : QObject(parent), m_socket(nullptr)
{
    QObject::connect(this, &ChatSession::error, this, &ChatSession::closed);
}

ChatSession::~ChatSession()
{
    delete m_socket;
}

QString ChatSession::lastError() const
{
    return m_lastError;
}

bool ChatSession::open(qintptr descriptor)
{
    if (m_socket)
        return false;

    QTcpSocket * socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(descriptor))  {
        QMetaObject::invokeMethod(this, "closed", Qt::QueuedConnection);
        return false;
    }

    initialize(socket);
    return true;
}

bool ChatSession::open(QTcpSocket * socket)
{
    Q_ASSERT(socket);
    if (m_socket || !socket || !socket->isValid())
        return false;

    // Take ownership of the socket
    socket->setParent(this);

    initialize(socket);
    return true;
}

bool ChatSession::open(const QHostAddress & address, quint16 port)
{
    if (m_socket || address.isNull())
        return false;

    QTcpSocket * socket = new QTcpSocket(this);
    socket->connectToHost(address, port);

    initialize(socket);
    return true;
}

void ChatSession::close()
{
    if (m_socket->isValid())
        m_socket->disconnectFromHost();
}

void ChatSession::send(const ChatMessage & message)
{
    Q_ASSERT(m_socket);
    if (!m_socket->isValid())
        return;

    QDataStream stream(m_socket);
    stream << QJsonDocument(message.toJson()).toJson();
}

void ChatSession::send(const ChatMessagePointer & messagePointer)
{
    const ChatMessage * message = messagePointer.data();
    if (message)
        send(*message);
}

void ChatSession::readData()
{
    QDataStream stream(m_socket);

    while (true) {
        // Read the JSON data
        stream.startTransaction();
        QByteArray jsonData;
        stream >> jsonData;
        if (!stream.commitTransaction())
            return;     // The message may come in chunks, so wait for more

        // Parse the JSON data
        QJsonParseError parseError;
        const QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData, &parseError);
        if(parseError.error != QJsonParseError::NoError)  {
            emit statusReport(QStringLiteral("Error parsing message: %1\n%2").arg(parseError.errorString()).arg(QString(jsonData)));
            return;
        }

        // Sanity check for the message
        if(!jsonDocument.isObject())  {
            emit statusReport(QStringLiteral("JSON message is of unexpected type"));
            return;
        }

        // Convert the JSON data to a message object & emit the appropriate signal
        decodeJson(jsonDocument.object());
    }
}

void ChatSession::setLastError(QAbstractSocket::SocketError error)
{
    switch (error)
    {
    case QAbstractSocket::RemoteHostClosedError:
    case QAbstractSocket::ProxyConnectionClosedError:
        m_lastError = QStringLiteral("The host terminated the connection");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        m_lastError = QStringLiteral("The host refused the connection");
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        m_lastError = QStringLiteral("The proxy refused the connection");
        break;
    case QAbstractSocket::ProxyNotFoundError:
        m_lastError = QStringLiteral("Could not find the proxy");
        break;
    case QAbstractSocket::HostNotFoundError:
        m_lastError = QStringLiteral("Could not find the server");
        break;
    case QAbstractSocket::SocketAccessError:
        m_lastError = QStringLiteral("You don't have permissions to execute this operation");
        break;
    case QAbstractSocket::SocketResourceError:
        m_lastError = QStringLiteral("Too many connections opened");
        break;
    case QAbstractSocket::SocketTimeoutError:
        m_lastError = QStringLiteral("Operation timed out");
        return;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        m_lastError = QStringLiteral("Proxy timed out");
        break;
    case QAbstractSocket::NetworkError:
        m_lastError = QStringLiteral("Unable to reach the network");
        break;
    case QAbstractSocket::UnknownSocketError:
        m_lastError = QStringLiteral("An unknown error occured");
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        m_lastError = QStringLiteral("Operation not supported");
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        m_lastError = QStringLiteral("Your proxy requires authentication");
        break;
    case QAbstractSocket::ProxyProtocolError:
        m_lastError = QStringLiteral("Proxy comunication failed");
        break;
    case QAbstractSocket::TemporaryError:
    case QAbstractSocket::OperationError:
        m_lastError = QStringLiteral("Operation failed, please try again");
        break;
    default:
        ;
    }

    emit statusReport(m_lastError);
}

void ChatSession::initialize(QTcpSocket * socket)
{
    Q_ASSERT(!m_socket);
    m_socket = socket;

    QObject::connect(socket, &QTcpSocket::connected, this, &ChatSession::opened);
    QObject::connect(socket, &QTcpSocket::disconnected, this, &ChatSession::closed);
    QObject::connect(socket, &QTcpSocket::readyRead, this, &ChatSession::readData);
    QObject::connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &ChatSession::setLastError);
    QObject::connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &ChatSession::error);

    QObject::connect(this, &ChatSession::closed, socket, &QObject::deleteLater);
}

void ChatSession::decodeJson(const QJsonObject & json)
{
    ChatMessagePointer message;
    switch (ChatMessage::type(json))
    {
    case ChatMessage::LoginType:
        message = new ChatMessageLogin();
        break;
    case ChatMessage::LoginStatusType:
        message = new ChatMessageLoginStatus();
        break;
    case ChatMessage::LogoutType:
        message = new ChatMessageLogout();
        break;
    case ChatMessage::TextType:
        message = new ChatMessageText();
        break;
    default:
        emit statusReport(QStringLiteral("Unknown message type"));
        return;
    }

    if (!message->fromJson(json))  {
        emit statusReport(QStringLiteral("Couldn't decode the JSON message"));
        return;
    }

    emit received(message);
}
