#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
class ChatClient;
class QStandardItemModel;
namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatWindow)
public:
    explicit ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow();
private:
    Ui::ChatWindow *ui;
    ChatClient* m_chatClient;
    QStandardItemModel* m_chatModel;
private slots:
    void attemptConnection();
    void connectedToServer();
    void attemptLogin(const QString& userName);
    void loggedIn();
    void loginFailed(const QString& reason);
    void messageReceived(const QString& sender, const QString& text);
    void sendMessage();
    void disconnectedFromServer();

};

#endif // CHATWINDOW_H
