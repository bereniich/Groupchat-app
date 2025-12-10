#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <netinet/in.h>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

#define BACKLOG 5
#define MAX_CLIENTS_NUM 10

class TCPServer 
{
private:
    int server_fd;
    int port;
    struct sockaddr_in address;
    socklen_t addrlen;

    std::vector<int> client_fds;
    std::mutex clients_mtx;
    std::condition_variable clients_cv;

    struct Message {
        int sender_fd;
        std::string text;
    };

    std::queue<Message> messages;
    std::mutex messages_mtx;
    std::condition_variable messages_cv;

    std::thread acceptThread;
    std::thread broadcastThread;

    bool running;

    void acceptClients();           // nit koja prihvata konekcije
    void receiveMessages(int clientFd); // nit koja prima poruke od klijenta
    void broadcastMessages();       // nit koja šalje poruke svima

    void removeClient(int clientFd);

public:
    TCPServer(int port);
    ~TCPServer();
    bool start();
    void stop();
};

#endif
