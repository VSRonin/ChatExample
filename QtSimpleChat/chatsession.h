#ifndef CHATSESSION_H
#define CHATSESSION_H

#include "qtsimplechat.h"

#include <QObject>
#include <QAbstractSocket>
#include <QPointer>
#include <QHostAddress>

class QTcpSocket;
class QJsonObject;

class ChatSessionPrivate;
class QTSIMPLECHAT_EXPORT ChatSession : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatSession)

public:
    explicit ChatSession(QObject * parent = nullptr);
    ~ChatSession() override;

    QString lastError() const;

public slots:
    bool open(qintptr);
    bool open(QTcpSocket *);
    bool open(const QHostAddress &, quint16 = 0);
    bool open(const QString &, quint16 = 0);
    void close();
    void send(const ChatMessage &);
    void send(const ChatMessagePointer &);

signals:
    void opened();
    void closed();
    void error();
    void received(const ChatMessagePointer &);
    void statusReport(const QString &);

private slots:
    void readData();
    void setLastError(QAbstractSocket::SocketError);

private:
    void initialize(QTcpSocket *);
    void decodeJson(const QJsonObject &);

private:
    QPointer<QTcpSocket> m_socket;
    QString m_lastError;
};

inline bool ChatSession::open(const QString & address, quint16 port)
{
    return open(QHostAddress(address), port);
}

#endif // CHATSESSION_H
