
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    client = new QWebSocket;
    connect(client, &QWebSocket::stateChanged, this, &MainWindow::clientConnectionStateChange);
    connect(client, &QWebSocket::textMessageReceived, this, &MainWindow::clientReceivedMessage);
    initServer();
}

void MainWindow::initServer()
{
    server = new QWebSocketServer(QStringLiteral("Echo Server"), QWebSocketServer::NonSecureMode, this);
    if (server->listen(QHostAddress("127.0.0.1"), 1502)) {
        connect(server, &QWebSocketServer::newConnection, this, &MainWindow::onNewConnection);
    }
}

void MainWindow::onNewConnection()
{
    QWebSocket *socket = server->nextPendingConnection();
    QByteArray header = socket->request().rawHeader("ClientId");
    ui->serverTextBrowser->append(QString("Client: %1 connected to the server").arg(header));
    connect(socket, &QWebSocket::textMessageReceived, this, &MainWindow::onMessageReceived);
}

void MainWindow::onMessageReceived(QString message)
{
    if (message.length() > 100) {
        ui->serverTextBrowser->append("Client: %1 sent message exceed 100 characters");
    }
    else {
        QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
        QByteArray header = socket->request().rawHeader("ClientId");
        QString replyMessage = message.append(QString("[ACK:<%1>]").arg(header));
        socket->sendTextMessage(replyMessage);
        QString logMessage = QDateTime::currentDateTime().toString("hh:mm:ss").append(" ").append(replyMessage);
        ui->serverTextBrowser->append(logMessage);
    }
}

void MainWindow::connectServer()
{
    if (client->state() == QAbstractSocket::ConnectedState) {
        ui->clientTextBrowser->append("Server is already connected");
    }
    else {
        QString address = "ws://";
        address.append(ui->addressTextEdit->displayText());
        address.append(QString(":1502"));
        QNetworkRequest request;
        request.setUrl(QUrl(address));
        request.setRawHeader(QByteArray("ClientId"), QByteArray(ui->clientTextEdit->displayText().toUtf8()));
        client->open(request);
    }
}

void MainWindow::clientReceivedMessage(QString message)
{

}

void MainWindow::clientConnectionStateChange()
{
    QString state = NULL;
    switch (client->state()) {
    case QAbstractSocket::UnconnectedState :
        state = "unconnected";
        break;
    case QAbstractSocket::HostLookupState :
        state = "host name hook up";
        break;
    case QAbstractSocket::ConnectingState :
        state = "connecting";
        break;
    case QAbstractSocket::ConnectedState :
        state = "connected";
        break;
    case QAbstractSocket::BoundState :
        state = "unconnected";
        break;
    case QAbstractSocket::ClosingState :
        state = "closing";
        break;
    case QAbstractSocket::ListeningState :
        state = "listening";
        break;
    default:
        state = "unknown";
        break;
    }
    QString logMessage = QDateTime::currentDateTime().toString("hh:mm:ss").append(" Connection status changed to ").append(state).append(" to server ").append(client->peerAddress().toString());
    ui->clientTextBrowser->append(logMessage);
}

void MainWindow::sendMessage()
{
    QString message = ui->sendTextEdit->displayText();
    if (client->state() != QAbstractSocket::ConnectedState) {
        ui->clientTextBrowser->append("Server is not connected");
    }
    else if (message.length() > 100) {
        ui->clientTextBrowser->append("Message exceed 100 characters");
    }
    else {
        QString logMessage = QDateTime::currentDateTime().toString("hh:mm:ss").append(" ").append(message);
        client->sendTextMessage(message);
        ui->clientTextBrowser->append(logMessage);
    }
}

void MainWindow::disconnectServer()
{
    if (client->state() == QAbstractSocket::ConnectedState) {
        client->close();
        ui->clientTextBrowser->append(QString("Disconnected from the server %1").arg(ui->addressTextEdit->displayText()));
    }
    else {
        ui->clientTextBrowser->append("Server is not connected");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_connectButton_clicked()
{
    connectServer();
}


void MainWindow::on_disconnectButton_clicked()
{
    disconnectServer();
}


void MainWindow::on_sendButton_clicked()
{
    sendMessage();
}

