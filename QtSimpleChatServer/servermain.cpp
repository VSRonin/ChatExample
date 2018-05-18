#include <QCoreApplication>
#include "chatserver.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ChatServer server;
    server.listen(QHostAddress::Any,1967);
    return a.exec();
}
