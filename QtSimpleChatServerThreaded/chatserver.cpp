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
    ,m_idealThreadCount(qMax(QThread::idealThreadCount(),1))
{
    m_availableThreads.reserve(m_idealThreadCount);
    m_threadsLoad.reserve(m_idealThreadCount);
}

ChatServer::~ChatServer()
{
    for(QThread* singleThread : m_availableThreads){
        singleThread->quit();
        singleThread->wait();
    }
}
void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    ServerWorker* worker = new ServerWorker;
    if(!worker->setSocketDescriptor(socketDescriptor)){
        worker->deleteLater();
        return;
    }
    int threadIdx = m_availableThreads.size();
    if (threadIdx<m_idealThreadCount) { //we can add a new thread
        m_availableThreads.append(new QThread(this));
        m_threadsLoad.append(1);
        m_availableThreads.last()->start();
    }
    else{
        // find the thread with the least amount of clients and use it
        threadIdx = std::distance(m_threadsLoad.cbegin(),std::min_element(m_threadsLoad.cbegin(), m_threadsLoad.cend()));
        ++m_threadsLoad[threadIdx];
    }
    worker->moveToThread(m_availableThreads.at(threadIdx));
    connect(m_availableThreads.at(threadIdx),&QThread::finished, worker, &QObject::deleteLater);
    const auto disconnectSlot = std::bind(&ChatServer::userDisconnected,this,worker,threadIdx);
    connect(worker, &ServerWorker::disconnectedFromClient, this, disconnectSlot);
    connect(worker, &ServerWorker::error, this, disconnectSlot);
    connect(worker,&ServerWorker::jsonReceived,this,std::bind(&ChatServer::jsonReceived,this,worker,std::placeholders::_1));
    m_clients.append(worker);
    qDebug() << "New client Connected";
}
void ChatServer::sendJson(ServerWorker *destination, const QByteArray &message)
{
    Q_ASSERT(destination);
    QTimer::singleShot(0,destination,std::bind(&ServerWorker::sendJson,destination,message));
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

void ChatServer::userDisconnected(ServerWorker *sender, int threadIdx)
{
    --m_threadsLoad[threadIdx];
    m_clients.removeAll(sender);
    const QString userName = sender->userName();
    if(!userName.isEmpty()){
        broadcast(R"({"type":"userdisconnected","username":")" + userName.toUtf8() + "\"}",nullptr);
        qDebug().noquote() << userName << " disconnected";
    }
    sender->deleteLater();
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

