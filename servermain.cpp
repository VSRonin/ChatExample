#include <QCoreApplication>
#include "chatserver.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ChatServer server;
    return a.exec();
}
