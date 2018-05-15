#include "serversocket.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonParseError>
ServerWorker::ServerWorker(QObject* parent)
    :QObject(parent)
    ,
{
    connect(this,&ServerSocket::readyRead,this,&ServerSocket::receiveJson);
}

void ServerSocket::sendJson(const QByteArray &jsonData)
{
    QDataStream socketStream(this);
    socketStream.setVersion(QDataStream::Qt_5_6);
    socketStream << jsonData;
}

void ServerSocket::receiveJson()
{
    QByteArray jsonData;
    QDataStream socketStream(this);
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


