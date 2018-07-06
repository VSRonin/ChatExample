#include <QApplication>

#include "chatserver.h"
#include "serverwindow.h"

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);

    ChatServer server;
    server.listen(QHostAddress::Any, 1967);

    ServerWindow serverWin;
    serverWin.show();

    return QApplication::exec();
}
