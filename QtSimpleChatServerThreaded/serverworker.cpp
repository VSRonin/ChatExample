#include "serverworker.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>
ServerWorker::ServerWorker(QObject* parent)
    :QObject(parent)
    , m_serverSocket(new QTcpSocket(this))
{
    connect(m_serverSocket,&QTcpSocket::readyRead,this,&ServerWorker::receiveJson);
    connect(m_serverSocket,&QTcpSocket::disconnected,this,&ServerWorker::disconnectedFromClient);
    connect(m_serverSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),this,&ServerWorker::error);
}


bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor, QAbstractSocket::SocketState socketState, QIODevice::OpenMode openMode)
{
    return m_serverSocket->setSocketDescriptor(socketDescriptor,socketState,openMode);
}

void ServerWorker::sendJson(const QByteArray &jsonData)
{
    qDebug().noquote() << "Sending to " << userName() << jsonData;
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << jsonData;
}

QString ServerWorker::userName() const
{
    m_userNameLock.lockForRead();
    const QString result = m_userName;
    m_userNameLock.unlock();
    return result;
}

void ServerWorker::setUserName(const QString &userName)
{
    m_userNameLock.lockForWrite();
    m_userName = userName;
    m_userNameLock.unlock();
}

void ServerWorker::receiveJson()
{
    QByteArray jsonData;
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    for(;;){
        socketStream.startTransaction();
        socketStream >> jsonData;
        if(socketStream.commitTransaction()){
            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData,&parseError);
            if(parseError.error == QJsonParseError::NoError)
                emit jsonReceived(jsonDoc);
            else
                qDebug() << "Invalid message: " << QString::fromUtf8(jsonData);
        }
        else{
            break;
        }
    }
}


