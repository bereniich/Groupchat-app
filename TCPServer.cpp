#include "TCPServer.h"
#include <unistd.h>
#include <iostream>
#include <cstring>

TCPServer::TCPServer(int port) : port(port), server_fd(-1), running(false) {
    addrlen = sizeof(address);
    memset(&address, 0, sizeof(address));
}

TCPServer::~TCPServer() {
    stop();
}

bool TCPServer::start() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) 
    {
        perror("Socket failed");
        return false;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
    {
        perror("setsockopt(SO_REUSEADDR) failed");
    }


    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) 
    {
        perror("Bind failed");
        return false;
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("Listen failed");
        return false;
    }

    running = true;

    acceptThread = std::thread(&TCPServer::acceptClients, this);
    broadcastThread = std::thread(&TCPServer::broadcastMessages, this);

    std::cout << "Server started on port " << port << std::endl;
    return true;
}

void TCPServer::stop() {
    if(!running) return;
    running = false;
    
    {
        std::unique_lock<std::mutex> lock(clients_mtx);        
        for(int fd : client_fds) {
            close(fd);
        }
            
        client_fds.clear();
    }

    {
        std::unique_lock<std::mutex> lock(messages_mtx);
        messages_cv.notify_all();
    }
    

    if(server_fd >= 0) {
        shutdown(server_fd, SHUT_RDWR);
        close(server_fd);
        server_fd = -1;
    }

    if(acceptThread.joinable()) acceptThread.join();
    if(broadcastThread.joinable()) broadcastThread.join();

    std::cout << "Server stopped!" << std::endl;
}

void TCPServer::acceptClients() {
    while (running)
    {
        int newClientFd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if(newClientFd < 0) continue;
        {
            std::unique_lock<std::mutex> lock(clients_mtx);

            if(client_fds.size() < MAX_CLIENTS_NUM) 
            {
                client_fds.push_back(newClientFd);
                std::cout << "Client connected: " << newClientFd << std::endl;
                std::thread(&TCPServer::receiveMessages, this, newClientFd).detach();
                
            } 
            else 
            {
                close(newClientFd);
                std::cout << "Max clients reached, connection refused" << std::endl;
            }
        }
    }
}

void TCPServer::receiveMessages(int clientFd) 
{
    char buffer[1024];

    while (running) 
    {
        memset(buffer, 0, sizeof(buffer));

        int valread = read(clientFd, buffer, sizeof(buffer));
        if (valread > 0) 
        {
            std::string new_msg(buffer, valread);
            std::unique_lock<std::mutex> lock(messages_mtx);            
            messages.push({clientFd, new_msg});
            
            messages_cv.notify_one();
        } 
        else if (valread == 0) {
            std::cout << "Client disconnected: " << clientFd << std::endl;
            close(clientFd);
            removeClient(clientFd);
            break;
        } 
        else 
        {
            perror("Read error");
            close(clientFd);
            removeClient(clientFd);
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void TCPServer::broadcastMessages() 
{
    while(running) {
        std::unique_lock<std::mutex> msg_lock(messages_mtx);
        while(messages.empty() && running) {
            messages_cv.wait(msg_lock);   
        }

        if(!running) return;

        std::string msg = messages.front().text;
        int sender = messages.front().sender_fd;
        messages.pop();

        msg_lock.unlock();

        std::unique_lock<std::mutex> clientsLock(clients_mtx);    
        for(int client : client_fds) 
        {
            if(client != sender) 
                send(client, msg.c_str(), msg.size(), 0);
        }    

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void TCPServer::removeClient(int clientFd) 
{
    std::unique_lock<std::mutex> lock(clients_mtx);

    for (auto it = client_fds.begin(); it != client_fds.end(); ) 
    {
        if (*it == clientFd) 
            it = client_fds.erase(it); 
        else 
            ++it;
    }
}