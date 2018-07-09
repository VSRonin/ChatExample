#include "chatmessage.h"

#include <QHash>

#include <QJsonObject>
#include <QJsonValue>

static QHash<QString, ChatMessage::Type> messageTypes = {
    { QStringLiteral("login"), ChatMessage::LoginType },
    { QStringLiteral("login-status"), ChatMessage::LoginStatusType },
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

ChatMessageLoginStatus::ChatMessageLoginStatus()
    : m_status(Success)
{
}

void ChatMessageLoginStatus::setStatus(Status status)
{
    m_status = status;
}

ChatMessageLoginStatus::Status ChatMessageLoginStatus::status() const
{
    return m_status;
}

void ChatMessageLoginStatus::setErrorText(const QString & errorText)
{
    m_errorText = errorText;
}

QString ChatMessageLoginStatus::errorText() const
{
    return m_errorText;
}

bool ChatMessageLoginStatus::fromJson(const QJsonObject & json)
{
    m_status = static_cast<Status>(json.value(QStringLiteral("status")).toInt());
    m_errorText = json.value(QStringLiteral("errorText")).toString();

    return m_status == Success || (m_status == Fail && !m_errorText.isEmpty());
}

QJsonObject ChatMessageLoginStatus::toJson() const
{
    QJsonObject json = ChatMessage::toJson();
    json.insert(QStringLiteral("status"), static_cast<int>(m_status));
    if (m_status == Fail)
        json.insert(QStringLiteral("errorText"), m_errorText);

    return json;
}

ChatMessage * ChatMessageLogin::clone() const
{
    return new ChatMessageLogin(*this);
}

QString ChatMessageLoginStatus::type() const
{
    return QStringLiteral("login-status");
}

ChatMessage * ChatMessageLoginStatus::clone() const
{
    return new ChatMessageLoginStatus(*this);
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
