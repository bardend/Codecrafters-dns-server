#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "DnsMessage.hpp"

int main() {
    int cnt = 1;
   while (cnt--) {
       // Receive data
       
        uint8_t buffer[512] = {
            0x17, 0x4F, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x0C, 0x63, 0x6F, 0x64,
            0x65, 0x63, 0x72, 0x61, 0x66, 0x74, 0x65, 0x72,
            0x73, 0x02, 0x69, 0x6F, 0x00, 0x00, 0x01, 0x00,
            0x01
        };

        int bytesRead = 33;


       for(int i = 0; i < bytesRead; i++) 
               std::cout << std::hex << (int)buffer[i] << " ";  // Imprime en hexadecimal

       std::cout << std::endl;

       std::cout << "============================" << std:: endl;
       

       DnsMessage Response = DnsMessage(buffer);

       vector<uint8_t> response = Response.GetBytes();

       cout << "La longitud es :: " << hex << (int)response.size() << endl;

       for(int i = 0; i < (int)response.size(); i++) {
           cout << hex << (int)response[i] << " ";
       }

       cout << endl;

       cout << "Terminacion de la respuesta " << endl;
   }


    return 0;
}
