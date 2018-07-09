#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include "qtsimplechat.h"
#include "ui_serverwindow.h"

#include <QWidget>

class ChatServer;
class ServerWindow : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ServerWindow)

public:
    explicit ServerWindow(QWidget * parent = nullptr);

signals:
    void toggleServer();

public slots:
    void logMessage(const QString &);
    void serverStopped();
    void serverStarted();

private:
    Ui::ServerWindow ui;
};

#endif // SERVERWINDOW_H
