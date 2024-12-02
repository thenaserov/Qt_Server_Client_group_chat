#include <QTcpServer>
#include <QTcpSocket>
#include <QCoreApplication>
#include <QList>
#include <QTextStream>

class ChatServer : public QTcpServer {
    Q_OBJECT

public:
    ChatServer(QObject *parent = nullptr) : QTcpServer(parent) {
        connect(this, &ChatServer::newConnection, this, &ChatServer::onNewConnection);
    }

    void startServer() {
        if (listen(QHostAddress::Any, 44444)) {
            QTextStream(stdout) << "Server started on port 44444...\n";
        } else {
            QTextStream(stderr) << "Server failed to start: " << errorString() << "\n";
        }
    }

private slots:
    void onNewConnection() {
        QTcpSocket *clientSocket = nextPendingConnection();
        if (clientSocket) {
            QTextStream(stdout) << "New client connected: " << clientSocket->peerAddress().toString() << "\n";
            clients.append(clientSocket);

            connect(clientSocket, &QTcpSocket::readyRead, this, &ChatServer::onReadyRead);
            connect(clientSocket, &QTcpSocket::disconnected, this, &ChatServer::onClientDisconnected);
        }
    }

    void onReadyRead() {
        QTcpSocket *senderSocket = qobject_cast<QTcpSocket *>(sender());
        if (!senderSocket)
            return;

        QByteArray message = senderSocket->readAll();
        QTextStream(stdout) << "Received: " << message << "\n";

        for (QTcpSocket *client : clients) {
            if (client != senderSocket) { // Do not echo back to the sender
                client->write(message);
            }
        }
    }

    void onClientDisconnected() {
        QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
        if (clientSocket) {
            QTextStream(stdout) << "Client disconnected: " << clientSocket->peerAddress().toString() << "\n";
            clients.removeAll(clientSocket);
            clientSocket->deleteLater();
        }
    }

private:
    QList<QTcpSocket *> clients;
};

#include "ChatServer.moc"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    ChatServer server;
    server.startServer();
    return app.exec();
}
