#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include "qtsimplechat.h"
#include "chatsession.h"
#include "ui_chatwindow.h"

#include <QWidget>
#include <QStandardItemModel>
#include <QTcpSocket>

class ChatMessageLogin;
class ChatMessageLoginStatus;
class ChatMessageLogout;
class ChatMessageText;

class ChatWindow : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatWindow)

public:
    explicit ChatWindow(QWidget *parent = nullptr);

private slots:
    void attemptConnection();
    void connectedToServer();
    void sendMessage();
    void error();
    void messageReceived(const ChatMessagePointer &);

private:
    void logInReceived(const ChatMessageLogin &);
    void logInStatusReceived(const ChatMessageLoginStatus &);
    void logOutReceived(const ChatMessageLogout &);
    void textReceived(const ChatMessageText &);

    void addMessage(const QString &, Qt::Alignment, const QBrush & = QBrush(Qt::black), const QFont & = QFont());

private:
    Ui::ChatWindow ui;
    QStandardItemModel m_chatModel;
    ChatSession m_session;
    QTcpSocket * m_socket;
    QString m_username, m_lastUserName;
};

#endif // CHATWINDOW_H
