#include <QApplication>
#include "serverwindow.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerWindow serverWin;
    serverWin.show();
    return a.exec();
}
