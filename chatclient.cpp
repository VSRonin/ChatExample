#include "chatclient.h"
#include <QTcpSocket>
#include <QDataStream>
#include <QJsonParseError>
#include <QJsonDocument>
ChatClient::ChatClient(QObject *parent)
    : QObject(parent)
    ,m_clientSocket(new QTcpSocket(this))
    , m_loggedIn(false)
{
    connect(m_clientSocket,&QTcpSocket::connected,this,&ChatClient::connected);
    connect(m_clientSocket,&QTcpSocket::disconnected,this,&ChatClient::disconnected);
    connect(m_clientSocket,&QTcpSocket::readyRead,this,&ChatClient::onReadyRead);
    connect(abstractSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),this,&ChatClient::error);
    connect(m_clientSocket,&QTcpSocket::disconnected,this,[this]()->void{m_loggedIn=false;});
}

void ChatClient::login(const QString &userName)
{
    if(m_clientSocket->state() == QAbstractSocket::ConnectedState){
        QDataStream clientStream;
        clientStream.setVersion(QDataStream::Qt_5_7);
        clientStream << (R"({"type":"login","username":")" + userName.toUtf8() + "\"})");
    }
}

void ChatClient::sendMessage(const QString &text)
{
    if(text.isEmpty())
        return;
    QDataStream clientStream;
    clientStream.setVersion(QDataStream::Qt_5_7);
    clientStream << (R"({"type":"message","text":")" + text.toUtf8() + "\"})");
}

void ChatClient::disconnectFromHost()
{
    m_clientSocket->disconnectFromHost();
}

void ChatClient::jsonReceived(const QJsonDocument &doc)
{
    if(!doc.isObject())
        return;
    const QJsonObject docObj = doc.object();
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    if(typeVal.isNull() || !typeVal.isString())
        return;
    if(typeVal.toString().compare(QLatin1String("login"),Qt::CaseInsensitive) == 0){
        if(m_loggedIn)
            return;
        const QJsonValue resultVal = docObj.value(QLatin1String("success"));
        if(resultVal.isNull() || !resultVal.isBool())
            return;
        const bool loginSuccess = resultVal.toBool();
        if(loginSuccess){
            emit loggedIn();
            return;
        }
        const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
        emit loginError(reasonVal.toString());
    }
    else if(typeVal.toString().compare(QLatin1String("message"),Qt::CaseInsensitive) == 0){
        const QJsonValue textVal = docObj.value(QLatin1String("text"));
        const QJsonValue senderVal = docObj.value(QLatin1String("sender"));
        if(textVal.isNull() || !textVal.isString())
            return;
        if(senderVal.isNull() || !senderVal.isString())
            return;
        emit messageReceived(senderVal.toString(),textVal.toString());
    }
}

ChatClient::connectToServer(const QHostAddress &address, quint16 port)
{
    m_clientSocket->connectToHost(address,port);
}

void ChatClient::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    for(;;){
        socketStream.startTransaction();
        socketStream >> jsonData;
        if(socketStream.commitTransaction()){
            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData,&parseError);
            if(parseError.error == QJsonParseError::NoError)
                jsonReceived(jsonDoc);
        }
        else{
            break;
        }
    }
}
