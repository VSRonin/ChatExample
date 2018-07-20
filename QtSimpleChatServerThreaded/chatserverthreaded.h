#ifndef CHATSERVERTHREADED_H
#define CHATSERVERTHREADED_H

#include "chatserver.h"
#include "threadpool.h"

class ChatServerThreaded : public ChatServer
{
public:
    explicit ChatServerThreaded(quint16 = 0, QObject * parent = nullptr);

protected:
    void incomingConnection(qintptr handle) override;

private:
    ThreadPool threadPool;
};

#endif // CHATSERVERTHREADED_H
