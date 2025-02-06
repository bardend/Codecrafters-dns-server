#include <iostream>
#include "DnsServer.hpp"
using namespace std;

int main(int argc, char* argv[]) {
    
    IPEndPoint ForwardServer;  

    if (argc == 3 && std::string(argv[1]) == "--resolver") {
        std::string arg = argv[2];
        size_t pos = arg.find(':');
        
        if (pos == std::string::npos) {
            std::string error = "Invalid resolver, expected ip:port, got " + arg;
            // Handle error
            return 1;
        }
        
        std::string ip = arg.substr(0, pos);
        std::string port = arg.substr(pos + 1);
        ForwardServer = IPEndPoint(ip, stoi(port));  // Inicialización posterior

    }

    cout << ForwardServer.Port << " " << ForwardServer.Address << endl;

    DnsServer server("127.0.0.1", 2053, ForwardServer);

    server.start();
    return 0;
}
