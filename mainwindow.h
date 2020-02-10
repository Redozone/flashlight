#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkSession>
#include <QTcpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void requestConnect();
    void connectedToHost();
    void readCommand();
    void displayError(QAbstractSocket::SocketError socketError);
    void enableButton();
    void sessionOpened();

private:
    Ui::MainWindow *ui;

    QTcpSocket *m_tcpSocket = nullptr;
    QDataStream m_in;
    qint8 m_currentType;

    QNetworkSession *m_networkSession = nullptr;

protected:
    void closeEvent(QCloseEvent * event);

};

#endif // MAINWINDOW_H
