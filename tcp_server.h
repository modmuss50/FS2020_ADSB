#pragma once

#include <vector>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace tcp {
    class SocketClient {
    public:
        SocketClient(int clientSocket, sockaddr_in clientAddress, socklen_t clientAddressSize);
        int getSocketID();

    private:
        int clientSocket;
        sockaddr_in clientAddress;
        socklen_t clientAddressSize;
    };

    class Server {

    public:
        Server(int);

        Server(Server const &server) = default;

        void start();

        void startNoneBlocking();

        void sendMessage(std::string message);

    private:

        void listen();

        int port, serverSocket;

        std::vector<SocketClient*> connectedClients;
    };
}