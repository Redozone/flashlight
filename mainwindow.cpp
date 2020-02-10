#include <QtNetwork>
#include <QMessageBox>
#include <QIntValidator>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tlvcommand.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_tcpSocket(new QTcpSocket(this)),
    m_currentType(-1)
{
    ui->setupUi(this);

    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(connectedToHost()));
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readCommand()));
    connect(m_tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &MainWindow::displayError);
    connect(ui->connectPushButton, &QAbstractButton::clicked, this, &MainWindow::requestConnect);
    connect(ui->hostLineEdit, &QLineEdit::textChanged, this, &MainWindow::enableButton);
    connect(ui->portLineEdit, &QLineEdit::textChanged, this, &MainWindow::enableButton);

    ui->portLineEdit->setValidator(new QIntValidator(1, 65535, this));


    m_in.setDevice(m_tcpSocket);
    m_in.setVersion(QDataStream::Qt_5_10);

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
                QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        m_networkSession = new QNetworkSession(config, this);
        connect(m_networkSession, &QNetworkSession::opened, this, &MainWindow::sessionOpened);

        ui->connectPushButton->setEnabled(false);
        ui->statusLabel->setText(tr("Opening network session."));
        m_networkSession->open();
    }

    QTimer::singleShot(0, this, SLOT(requestConnect()));
}

MainWindow::~MainWindow()
{
    if (m_tcpSocket)
        delete m_tcpSocket;
    if (m_networkSession)
        delete m_networkSession;

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *)
{
    if (m_tcpSocket)   {
        m_tcpSocket->disconnectFromHost();
    }
}

void MainWindow::requestConnect()
{
    ui->connectPushButton->setEnabled(false);
    m_tcpSocket->abort();
    m_tcpSocket->connectToHost(ui->hostLineEdit->text(), ui->portLineEdit->text().toUShort());
}

void MainWindow::connectedToHost()
{
    ui->statusLabel->setText(QString("Connected %1:%2")
                             .arg(ui->hostLineEdit->text()).arg(ui->portLineEdit->text()));
}

void MainWindow::readCommand()
{
    m_in.startTransaction();
    TLVcommand command;
    qint8 type = -1;
    m_in >> type;
    command.setType(static_cast<TLVcommand::pufCommand>(type));

    switch (command.type()) {
    case TLVcommand::ON:{
        QColor color = command.color();
        ui->lanternLabel->setStyleSheet(QString("background: %1").arg(color.name()));
        break;
    }
    case TLVcommand::OFF:{
        ui->lanternLabel->setStyleSheet("");
        break;
    }
    case TLVcommand::COLOR:{
        qint8 length = 0;
        m_in >> length;
        command.setLenght(length);
        QByteArray value = nullptr;
        for(int j = 0; j < length; ++j){
            quint8 character;
            m_in >> character;
            value.append(static_cast<char>(character));
        }
        if (value.length() == 3){
            QColor color;
            color.setNamedColor(QString("#") + value.toHex());
            command.setColor(color);
            ui->lanternLabel->setStyleSheet(QString("background: %1").arg(color.name()));
        }
        break;
    }
    default:
        ui->lanternLabel->setStyleSheet("background-color: rgb(0,0,0);");
        break;
    }

    if (!m_in.commitTransaction())
        return;

    if (type == m_currentType) {
        QTimer::singleShot(1000, this, &MainWindow::requestConnect);
        return;
    }
    m_currentType = type;
    ui->connectPushButton->setEnabled(true);
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Flashlight"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Flashlight"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Flashlight"),
                                 tr("The following error occurred: %1.")
                                 .arg(m_tcpSocket->errorString()));
    }

    ui->connectPushButton->setEnabled(true);
}

void MainWindow::enableButton()
{
    ui->connectPushButton->setEnabled((!m_networkSession || m_networkSession->isOpen()) &&
                                      !ui->hostLineEdit->text().isEmpty() &&
                                      !ui->portLineEdit->text().isEmpty());
}

void MainWindow::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = m_networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = m_networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();

    ui->statusLabel->setText(tr("This examples requires that you run the "
                                "Server example as well."));

    enableButton();
}
