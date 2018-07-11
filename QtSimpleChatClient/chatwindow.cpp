#include "chatwindow.h"
#include "chatmessage.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QHostAddress>

ChatWindow::ChatWindow(QWidget *parent)
    : QWidget(parent), m_chatModel(this), m_session(this)
{
    // Set up the form
    ui.setupUi(this);
    // The model for the messages will have 1 column
    m_chatModel.insertColumn(0);
    // Set the model as the data source for the list view
    ui.chatView->setModel(&m_chatModel);
    // Connect the connect button to a slot that will attempt the connection
    connect(ui.connectButton, &QPushButton::clicked, this, &ChatWindow::attemptConnection);
    // Connect the click of the "send" button and the press of the enter while typing to the slot that sends the message
    connect(ui.sendButton, &QPushButton::clicked, this, &ChatWindow::sendMessage);
    connect(ui.messageEdit, &QLineEdit::returnPressed, this, &ChatWindow::sendMessage);
    // Connect the signals from the chat session to the slots in this ui
    QObject::connect(&m_session, &ChatSession::received, this, &ChatWindow::messageReceived);
    QObject::connect(&m_session, &ChatSession::opened, this, &ChatWindow::connectedToServer);
    QObject::connect(&m_session, &ChatSession::error, this, &ChatWindow::error);
}

void ChatWindow::attemptConnection()
{
    // We ask the user for the address of the server, we use 127.0.0.1 (aka localhost) as default
    const QString hostAddress = QInputDialog::getText(this, tr("Chose Server"), tr("Server Address"), QLineEdit::Normal, QStringLiteral("127.0.0.1"));
    if (hostAddress.isEmpty())
        return; // the user pressed cancel or typed nothing
    // Disable the connect button to prevent the user clicking it again
    ui.connectButton->setEnabled(false);
    // Tell the client to connect to the host using the port 1967
    m_session.open(QHostAddress(hostAddress), 1967);
}

void ChatWindow::connectedToServer()
{
    // Once we connected to the server we ask the user for what username they would like to use
    const QString newUsername = QInputDialog::getText(this, tr("Chose Username"), tr("Username"));
    if (newUsername.isEmpty())  {
        // If the user clicked cancel or typed nothing, we just disconnect from the server
        m_session.close();
        // Reenable the connect button
        ui.connectButton->setEnabled(true);
        return;
    }

    // Store the selected user name
    m_username = newUsername;

    // Log into the server with the selected user name
    ChatMessageLogin * message = new ChatMessageLogin();
    message->setUsername(m_username);
    m_session.send(ChatMessagePointer(message));
}

void ChatWindow::sendMessage()
{
    // We are not logged in
    if (m_username.isEmpty())
        return;

    // Get the message text
    const QString messageText = ui.messageEdit->text();
    // We send the message that the user typed
    ChatMessageText * message = new ChatMessageText();
    message->setUsername(m_username);
    message->setText(messageText);
    m_session.send(ChatMessagePointer(message));
    // Now we add our own message to the list
    addMessage(messageText, Qt::AlignRight | Qt::AlignVCenter);
    // Clear the content of the message editor
    ui.messageEdit->clear();
}

void ChatWindow::error()
{
    // Notify the user there was an error
    addMessage(tr("Error: %1").arg(m_session.lastError()), Qt::AlignCenter, Qt::red);
    // Enable the button to connect to the server again
    ui.connectButton->setEnabled(true);
    // Disable the ui to send and display messages
    ui.sendButton->setEnabled(false);
    ui.messageEdit->setEnabled(false);
    ui.chatView->setEnabled(false);
}

void ChatWindow::messageReceived(const ChatMessagePointer & message)
{
    Q_ASSERT(message.data());
    switch (message->type())
    {
    case ChatMessage::LoginType:
        logInReceived(*reinterpret_cast<const ChatMessageLogin *>(message.data()));
        return;
    case ChatMessage::LoginStatusType:
        logInStatusReceived(*reinterpret_cast<const ChatMessageLoginStatus *>(message.data()));
        return;
    case ChatMessage::LogoutType:
        logOutReceived(*reinterpret_cast<const ChatMessageLogout *>(message.data()));
        return;
    case ChatMessage::TextType:
        textReceived(*reinterpret_cast<const ChatMessageText *>(message.data()));
        return;
    default:
        return;
    }
}

void ChatWindow::logInReceived(const ChatMessageLogin & message)
{
    // Store in the model the message to comunicate a user joined
    addMessage(tr("%1 Joined the Chat").arg(message.username()), Qt::AlignCenter, Qt::blue);
}

void ChatWindow::logInStatusReceived(const ChatMessageLoginStatus & message)
{
    if (message.status() == ChatMessageLoginStatus::Success)  {
        // Once we successully log in we enable the ui to display and send messages
        ui.sendButton->setEnabled(true);
        ui.messageEdit->setEnabled(true);
        ui.chatView->setEnabled(true);
    }
    else  {
        // The server rejected the login attempt, display the reason for the rejection in a message box
        QMessageBox::critical(this, tr("Error"), message.errorText());
        // allow the user to retry, execute the same slot as when just connected
        connectedToServer();
    }
}

void ChatWindow::logOutReceived(const ChatMessageLogout & message)
{
    // Store in the model the message to comunicate a user left
    addMessage(tr("%1 Left the Chat").arg(message.username()), Qt::AlignCenter, Qt::blue);
}

void ChatWindow::textReceived(const ChatMessageText & message)
{
    addMessage(QStringLiteral("%1: %2").arg(message.username()).arg(message.text()), Qt::AlignLeft | Qt::AlignVCenter);
}

void ChatWindow::addMessage(const QString & data, Qt::Alignment alignment, const QBrush & color)
{
    // Store the index of the new row to append to the model containing the messages
    const int newRow = m_chatModel.rowCount();
    // Insert a row for storing the message
    m_chatModel.insertRow(newRow);
    // Store the model index for the newly appended row
    QModelIndex row = m_chatModel.index(newRow, 0);

    // Store in the model the message text
    m_chatModel.setData(row, data);
    // Set the alignment for the text
    m_chatModel.setData(row, QVariant::fromValue<int>(alignment), Qt::TextAlignmentRole);
    // Set the color for the text
    m_chatModel.setData(row, color, Qt::ForegroundRole);

    // Scroll the view to display the new message
    ui.chatView->scrollToBottom();
}
