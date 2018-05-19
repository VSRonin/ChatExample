#include "chatsession.h"

#include <QTcpSocket>
#include <QDataStream>
#include <QBuffer>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

ChatSession::ChatSession(QObject * parent)
    : QObject(parent), socket(nullptr)
{
}

bool ChatSession::open(qintptr descriptor)
{
    socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(descriptor))  {
        QMetaObject::invokeMethod(this, &ChatSession::closed, Qt::QueuedConnection);
        return false;
    }

    initialize();
    return true;
}

bool ChatSession::open(QTcpSocket * channel)
{
    Q_ASSERT(channel);
    if (!channel)
        return false;

    socket = channel;

    initialize();
    return true;
}

void ChatSession::close()
{
    if (socket && socket->isValid())
        socket->disconnectFromHost();
}

void ChatSession::send(const QJsonObject & json)
{
    Q_ASSERT(socket);
    if (!socket)
        return;

    QDataStream stream(socket);
    stream << QJsonDocument(json).toJson();
}

void ChatSession::initialize()
{
    QObject::connect(socket, &QTcpSocket::connected, this, &ChatSession::opened);
    QObject::connect(socket, &QTcpSocket::disconnected, this, &ChatSession::closed);
    QObject::connect(socket, &QTcpSocket::readyRead, this, &ChatSession::readData);
}

void ChatSession::readData()
{
    data += socket->readAll();

    QBuffer buffer(&data);
    QDataStream stream(&buffer);

    // Read the JSON data
    stream.startTransaction();
    QByteArray jsonData;
    stream >> jsonData;
    if (!stream.commitTransaction())
        return;     // The message may come in chunks, so wait for more

    // Shorten the buffer with the read data
    data = data.mid(buffer.pos());

    // Parse the JSON data
    QJsonParseError parseError;
    const QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData, &parseError);
    if(parseError.error != QJsonParseError::NoError)  {
        qDebug().noquote() << QStringLiteral("Error parsing message: %1\n%2").arg(parseError.errorString()).arg(QString(jsonData));
        return;
    }

    // Sanity check for the message
    if(!jsonDocument.isObject())  {
        qDebug().noquote() << QStringLiteral("JSON message is of unexpected type");
        return;
    }

    QJsonObject json = jsonDocument.object();
    emit received(json);
}