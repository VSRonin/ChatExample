#include "serversocket.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonParseError>
ServerWorker::ServerWorker(QObject* parent)
    :QObject(parent)
    , m_serverSocket(new QTcpSocket(this))
{
    connect(m_serverSocket,&QTcpSocket::readyRead,this,&ServerWorker::receiveJson);
}

void ServerWorker::sendJson(const QByteArray &jsonData)
{
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_6);
    socketStream << jsonData;
}

void ServerWorker::receiveJson()
{
    QByteArray jsonData;
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_6);
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


