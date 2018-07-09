#include "chatsession.h"
#include "chatmessage.h"

#include <QTcpSocket>
#include <QDataStream>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

ChatSession::ChatSession(QObject * parent)
    : QObject(parent), socket(nullptr)
{
}

bool ChatSession::open(qintptr descriptor)
{
    socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(descriptor))  {
        QMetaObject::invokeMethod(this, "closed", Qt::QueuedConnection);
        return false;
    }

    initialize();
    return true;
}

bool ChatSession::open(QTcpSocket * channel)
{
    Q_ASSERT(channel);
    if (!channel)
        return false;

    socket = channel;

    initialize();
    return true;
}

void ChatSession::close()
{
    if (socket && socket->isValid())
        socket->disconnectFromHost();
}

void ChatSession::send(const ChatMessage & message)
{
    Q_ASSERT(socket);
    if (!socket)
        return;

    QDataStream stream(socket);
    stream << QJsonDocument(message.toJson()).toJson();
}

void ChatSession::send(const ChatMessagePointer & messagePointer)
{
    const ChatMessage * message = messagePointer.data();
    if (message)
        send(*message);
}

void ChatSession::initialize()
{
    QObject::connect(socket, &QTcpSocket::connected, this, &ChatSession::opened);
    QObject::connect(socket, &QTcpSocket::disconnected, this, &ChatSession::closed);
    QObject::connect(socket, &QTcpSocket::readyRead, this, &ChatSession::readData);
    QObject::connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &ChatSession::error);
}

void ChatSession::readData()
{
    QDataStream stream(socket);

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
            qDebug().noquote() << QStringLiteral("Error parsing message: %1\n%2").arg(parseError.errorString()).arg(QString(jsonData));
            return;
        }

        // Sanity check for the message
        if(!jsonDocument.isObject())  {
            qDebug().noquote() << QStringLiteral("JSON message is of unexpected type");
            return;
        }

        // Convert the JSON data to a message object & emit the appropriate signal
        decodeJson(jsonDocument.object());
    }
}

void ChatSession::decodeJson(const QJsonObject & json)
{
    ChatMessagePointer message;
    switch (ChatMessage::type(json))
    {
    case ChatMessage::LoginType:
        message = new ChatMessageLogin();
        break;
    case ChatMessage::LogoutType:
        message = new ChatMessageLogout();
        break;
    case ChatMessage::TextType:
        message = new ChatMessageText();
        break;
    default:
        qDebug().noquote() << QStringLiteral("Unknown message type.");
        return;
    }

    if (!message->fromJson(json))  {
        qDebug().noquote() << QStringLiteral("Couldn't decode the JSON message");
        return;
    }

    emit received(message);
}
