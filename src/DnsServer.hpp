#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <string>
#include "DnsMessage.hpp"

using namespace std;
class DnsServer {
    private:
        int udpSocket;
        struct sockaddr_in serverAddress;
        std::string ipAddress;
        int port;
        bool ShouldStop = false;

    public:
        DnsServer(const std::string& ip, int serverPort) 
                  : ipAddress(ip), port(serverPort) {

            udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
            if (udpSocket == -1) {
                throw std::runtime_error("Socket creation failed");
            }

            // Port reuse option
            int reuse = 1;
            setsockopt(udpSocket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
        }

        void bind() {
            // Prepare server address
            serverAddress = {
                .sin_family = AF_INET,
                .sin_port = htons(port),
                .sin_addr = { inet_addr(ipAddress.c_str()) }
            };

            if (::bind(udpSocket, reinterpret_cast<struct sockaddr*>(&serverAddress), 
                        sizeof(serverAddress)) != 0) {
                throw std::runtime_error("Bind failed");
            }

            std::cout << "Server listening on " << ipAddress << ":" << port << std::endl;
        }
        void start() {

            bind();
            while(!ShouldStop) {
                struct sockaddr_in clientAddress;
                socklen_t clientAddrLen = sizeof(clientAddress);

                vector<uint8_t>buffer(512);

                int bytesRead = recvfrom(udpSocket, buffer.data(), buffer.size(), 0, 
                                reinterpret_cast<struct sockaddr*>(&clientAddress), &clientAddrLen);
                if (bytesRead == -1) {
                    std::cerr << "Receive error" << std::endl;
                    continue;
                }

                buffer[bytesRead] = 0x00;


               for(int i = 0; i < bytesRead; i++) 
                       std::cout << std::hex << (int)buffer[i] << " ";  // Imprime en hexadecimal

               std::cout << std::endl;
               std::cout << "============================" << std:: endl;

               DnsMessage RequestMessage = DnsMessage(buffer);
               vector<uint8_t> response = ProcessRequest(RequestMessage).GetBytes();
               cout << "La longitud es :: " << hex << (int)response.size() << endl;

               for(int i = 0; i < (int)response.size(); i++) {
                   cout << hex << (int)response[i] << " ";
               }
               cout << endl;

               cout << "Vamos a ver el ProcessRequest :) " << endl;

               cout << "Terminacion de la respuesta " << endl;
               if (sendto(udpSocket, response.data(), response.size(), 0, 
                   reinterpret_cast<struct sockaddr*>(&clientAddress), sizeof(clientAddress)) == -1) {
                   perror("Failed to send response");
               }
            }
        }

        DnsMessage ProcessRequest(DnsMessage Request) {
            DnsMessage Response(Request.Header);

            Response.Header.QR = 1;
            Response.Header.AnswCount = 0;
            Response.Header.RespCode = (Response.Header.OpCode == 0x00 ? 0x00 : 0x04);

            for(auto Q: Request.Questions) {
                DnsRR A = DnsRR(Q);
                Response.Answers.push_back(A);
                Response.Header.AnswCount += 1;
                Response.Questions.push_back(Q);
            }
            return Response;
        }

        ~DnsServer() {
            close(udpSocket);
        }
        void Stop() {
            ShouldStop = true;
        }
};
