#include <QApplication>

#include "chatserverthreaded.h"
#include "serverwindow.h"

int main(int argc, char * argv[])
{
    QApplication application(argc, argv);

    ChatServerThreaded server(1967);

    ServerWindow serverWindow;
    serverWindow.show();

    QObject::connect(&application, &QApplication::aboutToQuit, &server, &ChatServer::stop);
    QObject::connect(&server, &ChatServer::started, &serverWindow, &ServerWindow::serverStarted);
    QObject::connect(&server, &ChatServer::stopped, &serverWindow, &ServerWindow::serverStopped);
    QObject::connect(&server, &ChatServer::statusReport, &serverWindow, &ServerWindow::logMessage);
    QObject::connect(&serverWindow, &ServerWindow::toggleServer, &server, &ChatServer::toggle);

    return QApplication::exec();
}
