#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include <QString>

class QJsonObject;

class ChatMessage
{
public:
    enum Type { UnknownType, LoginType, LogoutType, TextType };

public:
    static Type type(const QJsonObject &);

    void setUsername(const QString &);
    QString username() const;

    // Serialization
    virtual bool fromJson(const QJsonObject &);
    virtual QJsonObject toJson() const;

protected:
    virtual QString type() const = 0;

protected:
    QString user;
};

class ChatMessageLogin : public ChatMessage
{
public:
    QString type() const override;
};

class ChatMessageLogout : public ChatMessage
{
public:
    QString type() const override;
};

class ChatMessageText : public ChatMessage
{
public:
    void setText(const QString &);
    QString text() const;

    bool fromJson(const QJsonObject &) override;
    QJsonObject toJson() const override;

protected:
     QString type() const override;

protected:
    QString message;
};

#endif // CHATMESSAGE_H
