#include "chatmessage.h"

#include <QHash>

#include <QJsonObject>
#include <QJsonValue>

static QHash<QString, ChatMessage::Type> messageTypes = {
    { QStringLiteral("login"), ChatMessage::LoginType },
    { QStringLiteral("logout"), ChatMessage::LogoutType },
    { QStringLiteral("message"), ChatMessage::TextType }
};

ChatMessage::Type ChatMessage::type(const QJsonObject & json)
{
    QString messageType = json.value(QStringLiteral("type")).toString();
    return messageTypes.value(messageType, UnknownType);
}

ChatMessage::Type ChatMessage::type(const ChatMessage & message)
{
    return messageTypes.value(message.type(), UnknownType);
}

ChatMessage::Type ChatMessage::type(const ChatMessagePointer & message)
{
    return messageTypes.value(message->type(), UnknownType);
}

ChatMessage::Type ChatMessage::type(const ChatMessage * message)
{
    return messageTypes.value(message->type(), UnknownType);
}

ChatMessage::~ChatMessage()
{
}

void ChatMessage::setUsername(const QString & username)
{
    user = username;
}

QString ChatMessage::username() const
{
    return user;
}

bool ChatMessage::fromJson(const QJsonObject & json)
{
    Q_ASSERT(type() == type(json));
    if (type() != type(json))
        return false;

    user = json.value(QStringLiteral("username")).toString();
    return !user.isEmpty();
}

QJsonObject ChatMessage::toJson() const
{
    QJsonObject result = {
        { QStringLiteral("type"), type() },
        { QStringLiteral("username"), user }
    };

    return result;
}

QString ChatMessageLogin::type() const
{
    return QStringLiteral("login");
}

ChatMessage * ChatMessageLogin::clone() const
{
    return new ChatMessageLogin(*this);
}

QString ChatMessageLogout::type() const
{
    return QStringLiteral("logout");
}

ChatMessage * ChatMessageLogout::clone() const
{
    return new ChatMessageLogout(*this);
}

void ChatMessageText::setText(const QString & text)
{
    message = text;
}

QString ChatMessageText::text() const
{
    return message;
}

bool ChatMessageText::fromJson(const QJsonObject & json)
{
    if (!ChatMessage::fromJson(json))
        return false;

    message = json.value(QStringLiteral("text")).toString();
    return !message.isEmpty();
}

QJsonObject ChatMessageText::toJson() const
{
    QJsonObject result = ChatMessage::toJson();
    result.insert(QStringLiteral("text"), message);

    return result;
}

QString ChatMessageText::type() const
{
    return QStringLiteral("message");
}

ChatMessage * ChatMessageText::clone() const
{
    return new ChatMessageText(*this);
}
