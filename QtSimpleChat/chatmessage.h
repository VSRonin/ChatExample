#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include "qtsimplechat.h"

#include <QString>
#include <QSharedData>

class QJsonObject;

class QTSIMPLECHAT_EXPORT ChatMessage : public QSharedData
{
    friend ChatMessagePointer;

public:
    enum Type { UnknownType, LoginType, LoginStatusType, LogoutType, TextType };

public:
    ChatMessage() = default;
    virtual ~ChatMessage();

protected:
    ChatMessage(const ChatMessage &) = default;
    ChatMessage & operator = (const ChatMessage &) = default;

public:
    static Type type(const QJsonObject &);
    static Type type(const ChatMessage &);
    static Type type(const ChatMessagePointer &);
    static Type type(const ChatMessage *);

    void setUsername(const QString &);
    QString username() const;

    // Serialization
    virtual bool fromJson(const QJsonObject &);
    virtual QJsonObject toJson() const;

protected:
    virtual QString type() const = 0;
    virtual ChatMessage * clone() const = 0;

protected:
    QString user;
};

// Provide the correct copying for the message through the shared pointer
template <>
inline ChatMessage * ChatMessagePointer::clone()
{
    return d->clone();
}

class QTSIMPLECHAT_EXPORT ChatMessageLogin : public ChatMessage
{
public:
    ChatMessageLogin() = default;

private:
    ChatMessageLogin(const ChatMessageLogin &) = default;

protected:
    QString type() const override;
    ChatMessage * clone() const override;
};

class QTSIMPLECHAT_EXPORT ChatMessageLoginStatus : public ChatMessage
{
public:
    enum Status { Fail, Success };

    ChatMessageLoginStatus();

    void setStatus(Status);
    Status status() const;

    void setErrorText(const QString &);
    QString errorText() const;

    bool fromJson(const QJsonObject &) override;
    QJsonObject toJson() const override;

private:
    ChatMessageLoginStatus(const ChatMessageLoginStatus &) = default;

protected:
    QString type() const override;
    ChatMessage * clone() const override;

    Status m_status;
    QString m_errorText;
};

class QTSIMPLECHAT_EXPORT ChatMessageLogout : public ChatMessage
{
public:
    ChatMessageLogout() = default;

private:
    ChatMessageLogout(const ChatMessageLogout &) = default;

protected:
    QString type() const override;
    ChatMessage * clone() const override;
};

class QTSIMPLECHAT_EXPORT ChatMessageText : public ChatMessage
{
public:
    ChatMessageText() = default;

private:
    ChatMessageText(const ChatMessageText &) = default;

public:
    void setText(const QString &);
    QString text() const;

    bool fromJson(const QJsonObject &) override;
    QJsonObject toJson() const override;

protected:
    QString type() const override;
    ChatMessage * clone() const override;

    QString message;
};

#endif // CHATMESSAGE_H
