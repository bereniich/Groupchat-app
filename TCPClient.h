#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>
#include <netinet/in.h>
#include <thread>
#include <atomic>

class TCPClient {
private:
    int sock;
    struct sockaddr_in serv_addr;
    std::string server_ip;
    int port;
    std::string name;

    std::thread sendThread;
    std::thread receiveThread;

    std::atomic<bool> disconnect;

public:
    TCPClient(const std::string& ip, int port, std::string name);
    ~TCPClient();
    bool connectToServer();
    void sendMessage();
    void receiveMessage();
};

#endif
