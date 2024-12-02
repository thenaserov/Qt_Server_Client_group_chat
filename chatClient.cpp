#include <QTcpSocket>
#include <QCoreApplication>
#include <QTextStream>
#include <QTimer>

class ChatClient : public QObject {
    Q_OBJECT

public:
    ChatClient(QObject *parent = nullptr) : QObject(parent), socket(new QTcpSocket(this)) {
        connect(socket, &QTcpSocket::connected, this, &ChatClient::onConnected);
        connect(socket, &QTcpSocket::readyRead, this, &ChatClient::onReadyRead);
        connect(socket, &QTcpSocket::disconnected, this, &ChatClient::onDisconnected);
    }

    void connectToServer(const QString &host, quint16 port) {
        QTextStream(stdout) << "Connecting to " << host << ":" << port << "...\n";
        socket->connectToHost(host, port);
    }

private slots:
    void onConnected() {
        QTextStream(stdout) << "Connected to the server.\n";
        QTextStream(stdout) << "Type messages to send, or 'exit' to quit.\n";

        QTextStream input(stdin);
        while (true) {
            QTextStream(stdout) << "> ";
            QString message = input.readLine();
            if (message.toLower() == "exit") {
                socket->disconnectFromHost();
                break;
            }
            socket->write(message.toUtf8());
        }
    }

    void onReadyRead() {
        QByteArray message = socket->readAll();
        QTextStream(stdout) << "Message from server: " << message << "\n";
    }

    void onDisconnected() {
        QTextStream(stdout) << "Disconnected from server.\n";
        QCoreApplication::quit();
    }

private:
    QTcpSocket *socket;
};

#include "ChatClient.moc"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    ChatClient client;
    client.connectToServer("127.0.0.1", 44444);

    return app.exec();
}
