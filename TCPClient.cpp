#include "TCPClient.h"
#include "Huffman.h"
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

TCPClient::TCPClient(const std::string& ip, int port, std::string name) : 
    server_ip(ip), port(port), name(name), sock(-1), disconnect(false)
{
    memset(&serv_addr, 0, sizeof(serv_addr));
}

TCPClient::~TCPClient() 
{
    disconnect = true;
    if (sendThread.joinable()) sendThread.join();
    if (receiveThread.joinable()) receiveThread.join();
    if (sock >= 0) close(sock);
}

bool TCPClient::connectToServer() 
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) 
    {
        perror("Socket creation error");
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr) <= 0) 
    {
        perror("Invalid address/ Address not supported");
        return false;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
    {
        perror("Connection Failed");
        return false;
    }

    std::cout << "Connected to server!" << std::endl;
    std::cout << "Press ENTER to disconnect" << std::endl;

    sendThread = std::thread(&TCPClient::sendMessage, this);
    receiveThread = std::thread(&TCPClient::receiveMessage, this);

    sendThread.join();
    receiveThread.join();

    return true;
}

void TCPClient::sendMessage() 
{
    std::string msg;

    while (!disconnect)
    {
        getline(std::cin, msg);

        if(msg.empty()) 
        {
            std::cout << "Disconnecting from server..." << std::endl;
            std::string s = "User @" + name + " diconnected from server.";
            disconnect = true;
            send(sock, s.c_str(), s.size(), 0);
            shutdown(sock, SHUT_RDWR);
            break;
        }

        std::string formated_msg = "@" + name + ": " + msg;
        Huffman h;
        std::string compressedMsg = h.compress(formated_msg);

        send(sock, compressedMsg.c_str(), compressedMsg.size(), 0);
    }
}

void TCPClient::receiveMessage() 
{
    char buffer[1024];
    while(!disconnect)
    {
        memset(buffer, 0, sizeof(buffer));

        int valread = read(sock, buffer, sizeof(buffer));

        Huffman h;
        if (valread > 0) 
        {
            std::cout << "\r" << h.decompress(buffer) << "\n> " << std::flush;
        } 
        else if (valread == 0) 
        {
            std::cout << "Connection closed.\n";
            disconnect = true;
            shutdown(sock, SHUT_RDWR);
            break;
        } 
        else 
        {
            perror("Read error");
            disconnect = true;
            break;
        }
    }
}

