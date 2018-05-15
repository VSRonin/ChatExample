#include "serverworker.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonParseError>
ServerWorker::ServerWorker(QObject* parent)
    :QObject(parent)
    , m_serverSocket(new QTcpSocket(this))
{
    connect(m_serverSocket,&QTcpSocket::readyRead,this,&ServerWorker::receiveJson);
}

bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor, QAbstractSocket::SocketState socketState, QIODevice::OpenMode openMode)
{
    m_serverSocket->setSocketDescriptor(socketDescriptor,socketState,openMode);
}

void ServerWorker::sendJson(const QByteArray &jsonData)
{
    if(m_serverSocket)
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << jsonData;
}

QString ServerWorker::userName() const
{
    return m_userName;
}

void ServerWorker::setUserName(const QString &userName)
{
    m_userName = userName;
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
        }
        else{
            break;
        }
    }
}


