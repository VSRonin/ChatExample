#include "serverwindow.h"
#include "ui_serverwindow.h"

#include "chatserver.h"

ServerWindow::ServerWindow(QWidget * parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    QObject::connect(ui.startStopButton, &QPushButton::clicked, this, &ServerWindow::toggleServer);
}

void ServerWindow::serverStarted()
{
    ui.startStopButton->setText(tr("Stop Server"));
}

void ServerWindow::serverStopped()
{
    ui.startStopButton->setText(tr("Start Server"));
}

void ServerWindow::logMessage(const QString & msg)
{
    ui.logEditor->appendPlainText(msg);
}
