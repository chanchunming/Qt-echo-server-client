
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();

    void on_disconnectButton_clicked();

    void on_sendButton_clicked();

private:
    Ui::MainWindow *ui;
    QWebSocketServer *server;
    QWebSocket *client;
    void initServer();
    void onNewConnection();
    void onMessageReceived(QString);
    void connectServer();
    void clientReceivedMessage(QString);
    void clientConnectionStateChange();
    void sendMessage();
    void disconnectServer();
};

#endif // MAINWINDOW_H
