#include "chatwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    ChatWindow chatWindow;
    chatWindow.show();

    return QApplication::exec();
}
