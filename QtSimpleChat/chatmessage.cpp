#include "chatmessage.h"

#include <QVector>
#include <QHash>
#include <QString>

#include <QJsonObject>
#include <QJsonValue>

struct MessageTypes
{
public:
    MessageTypes();

    static QString toString(int);
    static ChatMessage::Type fromString(const QString &);

private:
    QVector<QString> typeToString;
    QHash<QString, ChatMessage::Type> stringToType;
} __messageTypes;

MessageTypes::MessageTypes()
{
    typeToString = {
        QStringLiteral("login"),
        QStringLiteral("login-status"),
        QStringLiteral("logout"),
        QStringLiteral("message")
    };

    qint32 size = typeToString.size();
    stringToType.reserve(size);

    for (qint32 i = 0; i < size; i++)
        stringToType.insert(typeToString.at(i), static_cast<ChatMessage::Type>(i));
}

inline QString MessageTypes::toString(int type)
{
    return type < 0 || type >= __messageTypes.typeToString.size() ? QString() : __messageTypes.typeToString.at(type);
}

inline ChatMessage::Type MessageTypes::fromString(const QString & string)
{
    return __messageTypes.stringToType.value(string, ChatMessage::UnknownType);
}

// ---------------------------------------------------------------------------------------------------------------------------------------- //

ChatMessage::Type ChatMessage::type(const QJsonObject & json)
{
    QString messageType = json.value(QStringLiteral("type")).toString();
    return MessageTypes::fromString(messageType);
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
        { QStringLiteral("type"), MessageTypes::toString(type()) },
        { QStringLiteral("username"), user }
    };

    return result;
}

ChatMessage::Type ChatMessageLogin::type() const
{
    return LoginType;
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

ChatMessage::Type ChatMessageLoginStatus::type() const
{
    return LoginStatusType;
}

ChatMessage * ChatMessageLoginStatus::clone() const
{
    return new ChatMessageLoginStatus(*this);
}

ChatMessage::Type ChatMessageLogout::type() const
{
    return LogoutType;
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

ChatMessage::Type ChatMessageText::type() const
{
    return TextType;
}

ChatMessage * ChatMessageText::clone() const
{
    return new ChatMessageText(*this);
}
