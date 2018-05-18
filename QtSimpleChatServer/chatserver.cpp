#include "chatserver.h"
#include "serverworker.h"
#include <QThread>
#include <functional>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include <QTimer>
ChatServer::ChatServer(QObject *parent)
    :QTcpServer(parent)
{}

void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    ServerWorker* worker = new ServerWorker(this);
    if(!worker->setSocketDescriptor(socketDescriptor)){
        worker->deleteLater();
        return;
    }
    connect(worker, &ServerWorker::disconnectedFromClient, this, std::bind(&ChatServer::userDisconnected,this,worker));
    connect(worker, &ServerWorker::error, this, std::bind(&ChatServer::userError,this,worker));
    connect(worker,&ServerWorker::jsonReceived,this,std::bind(&ChatServer::jsonReceived,this,worker,std::placeholders::_1));
    m_clients.append(worker);
    qDebug() << "New client Connected";
}
void ChatServer::sendJson(ServerWorker *destination, const QByteArray &message)
{
    Q_ASSERT(destination);
    destination->sendJson(message);
}
void ChatServer::broadcast(const QByteArray &message, ServerWorker *exclude)
{
    for(ServerWorker* worker : m_clients){
        Q_ASSERT(worker);
        if(worker==exclude)
            continue;
        sendJson(worker,message);
    }
}

void ChatServer::jsonReceived(ServerWorker *sender, const QJsonDocument &doc)
{
    Q_ASSERT(sender);
    qDebug().noquote() << "json received" << doc.toJson();
    if(sender->userName().isEmpty())
        return jsonFromLoggedOut(sender,doc);
    jsonFromLoggedIn(sender,doc);
}

void ChatServer::userDisconnected(ServerWorker *sender)
{
    m_clients.removeAll(sender);
    const QString userName = sender->userName();
    if(!userName.isEmpty()){
        broadcast(R"({"type":"userdisconnected","username":")" + userName.toUtf8() + "\"}",nullptr);
        qDebug().noquote() << userName << " disconnected";
    }
    sender->deleteLater();
}

void ChatServer::userError(ServerWorker *sender)
{
    Q_UNUSED(sender)
    qDebug().noquote() << "Error from " << sender->userName();
}

void ChatServer::jsonFromLoggedOut(ServerWorker *sender, const QJsonDocument &doc)
{
    Q_ASSERT(sender);
    Q_ASSERT(!doc.isNull());
    if(!doc.isObject())
        return;
    const QJsonObject docObj = doc.object();
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    if(typeVal.isNull() || !typeVal.isString())
        return;
    if(typeVal.toString().compare(QLatin1String("login"),Qt::CaseInsensitive) != 0)
        return;
    const QJsonValue usernameVal = docObj.value(QLatin1String("username"));
    if(usernameVal.isNull() || !usernameVal.isString())
        return;
    const QString newUserName = usernameVal.toString().simplified();
    if(newUserName.isEmpty())
        return;
    for(ServerWorker* worker : qAsConst(m_clients)){
        if(worker==sender)
            continue;
        if(worker->userName().compare(newUserName,Qt::CaseInsensitive)==0){
            const QByteArray message = QByteArrayLiteral(R"({"type":"login","success":false,"reason":"duplicate username"})");
            sendJson(sender,message);
            return;
        }
    }
    sender->setUserName(newUserName);
    sendJson(sender,QByteArrayLiteral(R"({"type":"login","success":true})"));
    broadcast(R"({"type":"newuser","username":")" + newUserName.toUtf8() + "\"}",sender);
}

void ChatServer::jsonFromLoggedIn(ServerWorker *sender, const QJsonDocument &doc)
{
    Q_ASSERT(sender);
    Q_ASSERT(!doc.isNull());
    if(!doc.isObject())
        return;
    const QJsonObject docObj = doc.object();
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    if(typeVal.isNull() || !typeVal.isString())
        return;
    if(typeVal.toString().compare(QLatin1String("message"),Qt::CaseInsensitive) != 0)
        return;
    const QJsonValue textVal = docObj.value(QLatin1String("text"));
    if(textVal.isNull() || !textVal.isString())
        return;
    const QString text = textVal.toString().trimmed();
    if(text.isEmpty())
        return;
    broadcast(R"({"type":"message","text":")"+ text.toUtf8() + R"(","sender":")" + sender->userName().toUtf8() + "\"}",sender);
}


