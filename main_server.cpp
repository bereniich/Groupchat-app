#include "TCPServer.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() 
{
    TCPServer server(12346);
    if (!server.start()) 
    {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }

    std::cout << "Press Enter to stop server..." << std::endl;
    std::cin.get(); // čekaj da korisnik pritisne Enter

    server.stop();
    return 0;
}
