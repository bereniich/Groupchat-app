#include "TCPClient.h"
#include <iostream>
#include <string>
#include <cstdlib> 

int main(int argc, char* argv[]) 
{
    if (argc != 4) 
    {
        std::cerr << "Error! Input shoud be in this format: " << argv[0] << " <IP> <PORT> <NAME>\n";
        return -1;
    }

    std::string ip = argv[1];
    int port = std::atoi(argv[2]);
    std::string name = argv[3];

    TCPClient client(ip, port, name); // prosledjujemo ime
    if (!client.connectToServer()) 
        return -1;

    return 0;
}
