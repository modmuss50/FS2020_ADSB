#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <algorithm>

#include "tcp_server.h"

// This is kinda crap, prob not thread safe but does the job
namespace tcp {

    Server::Server(int portNumber) {
        port = portNumber;
    }

    void Server::start() {

        //Setup windows sockets
        WSADATA data;
        int r = WSAStartup(MAKEWORD(2, 2), &data);
        if (r != NO_ERROR) {
            std::cout << "Failed to start windows socket connection " << r << std::endl;
            return;
        }

        sockaddr_in serverAddress{};
        memset((char*)&serverAddress, '\0', sizeof(serverAddress));

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddress.sin_port = htons(port);

        //Create the network socket, using IPV4 (AF_INET) and TCP (SOCK_STREAM)
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);

        if(serverSocket < 0) {
            std::cerr << "Failed to setup socket" << std::endl;
            exit(1);
        }

        //Bind the socket to the server address
        int bindResult = ::bind(serverSocket,(struct sockaddr *)&serverAddress,sizeof(serverAddress));
        int listenResult = ::listen(serverSocket, 5);

        if(bindResult != 0 || listenResult != 0) {
            std::cerr << "Failed to bind or listen to socket" << std::endl;
            exit(1);
        }

        std::thread listenThread (&Server::listen, this);
        listenThread.detach();
    }

    void Server::listen() {
        while (true) {
            sockaddr_in clientAddress{};
            socklen_t clientAddressSize = sizeof(clientAddress);

            int clientSocket = accept( serverSocket, (struct sockaddr *)&clientAddress, &clientAddressSize);

            if (clientSocket < 0) {
                continue;
            }

            auto* client = new SocketClient(clientSocket, clientAddress, clientAddressSize);

            connectedClients.push_back(client);
        }
        closesocket(serverSocket);
    }

    void Server::sendMessage(std::string message) {

        for(auto client : connectedClients) {
            ::send(client->getSocketID(), message.c_str(), strlen(message.c_str()), 0);
        }

    }

    void Server::startNoneBlocking() {
        std::thread serverThread (&Server::start, this);
        serverThread.detach();
    }

    SocketClient::SocketClient(int clientSocketIn, sockaddr_in clientAddressIn, socklen_t clientAddressSizeIn) {
        clientSocket = clientSocketIn;
        clientAddress = clientAddressIn;
        clientAddressSize = clientAddressSizeIn;
    }

    int SocketClient::getSocketID() {
        return clientSocket;
    }
}