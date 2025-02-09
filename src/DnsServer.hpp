#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <string>
#include <vector>


#include "DnsMessage.hpp"

using namespace std;
class DnsServer {
    private:
        string ipAddress;
        int port;
        bool ShouldStop = false;
        IPEndPoint ForwardServer;
        int LocalSocket;

    public:
        DnsServer(const std::string& ip, int serverPort, IPEndPoint forwardServer) 
                  : ipAddress(ip), port(serverPort), ForwardServer(forwardServer) {}

        void start() {
            LocalSocket = socket(AF_INET, SOCK_DGRAM, 0);
            if (LocalSocket == -1) {
                  std::cerr << "Error al crear LocalSocket UDP\n";
            }

            // Port reuse option
            // int reuse = 1;
            // setsockopt(udpSocket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));

            sockaddr_in ServerLocal {};
            ServerLocal.sin_family = AF_INET;
            ServerLocal.sin_addr.s_addr = inet_addr(ipAddress.c_str());
            ServerLocal.sin_port = htons(port);


            if (bind(LocalSocket, reinterpret_cast<struct sockaddr*>(&ServerLocal), 
                        sizeof(ServerLocal)) != 0) {
                std::cerr << "Error al enlazar BIND\n";
                close(LocalSocket);
            }

            while(!ShouldStop) {
                cout <<"===============================VAMOS A EMPEZAR============================="<< endl;
                sockaddr_in LocalClient{};
                socklen_t LocalClientLen = sizeof(LocalClient);

                vector<uint8_t>buffer(512);

                ssize_t bytesRead = recvfrom(LocalSocket, buffer.data(), buffer.size(), 0, 
                                reinterpret_cast<struct sockaddr*>(&LocalClient), &LocalClientLen);

                if (bytesRead == -1) {
                    std::cerr << "Receive error" << std::endl;
                    continue;
                }

               DnsMessage RequestMessage = DnsMessage(buffer, 0);

               vector<uint8_t> response = ProcessRequest(RequestMessage).GetBytes();

               for(int i = 0; i < (int)response.size(); i++) 
                   cout << hex << (int)response[i] << " ";
               cout << endl;

               cout << "=========================TERMINAMOS ENVIAR==============================" << endl;

               if (sendto(LocalSocket, response.data(), response.size(), 0, 
                   reinterpret_cast<struct sockaddr*>(&LocalClient), sizeof(LocalClient)) == -1) {
                   perror("Failed to send response");
               }
            }
        }


        /* This server respond only one Question */
        DnsMessage ForwardRequest(DnsMessage Request) {

            int ForwardSocket = socket(AF_INET, SOCK_DGRAM, 0);
            if(ForwardSocket == -1) 
                std::cerr << "Error al crear socket de reenvío\n";


            sockaddr_in ConfigForward{};
            ConfigForward.sin_family = AF_INET;
            ConfigForward.sin_addr.s_addr = inet_addr(ForwardServer.Address.c_str());
            ConfigForward.sin_port = htons(ForwardServer.Port); 

            vector<uint8_t> Query = Request.GetBytes();
            ssize_t bytesSent = sendto(ForwardSocket, Query.data(), Query.size(), 0,
                                reinterpret_cast<struct sockaddr*>(&ConfigForward), sizeof(ConfigForward));

            if (bytesSent < 0) {
                cout << "Error enviando datos: " << strerror(errno) << endl;
                close(ForwardSocket);
            }

            sockaddr_in CurrentAddr{};
            socklen_t CurrentAddrLen = sizeof(CurrentAddr);

            vector<uint8_t>buffer(512);

            ssize_t bytesRead = recvfrom(ForwardSocket, buffer.data(), buffer.size(), 0, 
                            reinterpret_cast<struct sockaddr*>(&CurrentAddr), &CurrentAddrLen);

            cout << "Porque no recivo nada :(" << endl;

            if (bytesRead < 0) {
                cout << "Error recibiendo datos: " << strerror(errno) << endl;
                close(ForwardSocket);
            }

            cout << "Mensaje recivido desde servidor codecrafeters:" << endl;
            for(int i = 0; i < bytesRead; i++)
                cout << hex << (int)buffer[i] << " ";  // Imprime en hexadecimal
            cout << endl;

            DnsMessage ResponseGoogle = DnsMessage(buffer, 1);
            close(ForwardSocket);
            return ResponseGoogle;
        }


        DnsMessage ProcessRequest(DnsMessage Request) {

            DnsMessage RetResponse = DnsMessage(Request.Header);
            RetResponse.Header.QR = 1;
            RetResponse.Header.QuesCount = 0;

            int sum_pos = 0;
            for(auto Q: Request.Questions) {
                DnsMessage Temporal = DnsMessage(Request.Header);
                Temporal.Header.QuesCount = 1;
                Temporal.Questions.push_back(Q);
                DnsMessage SplitResponse = ForwardRequest(Temporal);

                RetResponse.Questions.push_back(SplitResponse.Questions.back());
                RetResponse.Answers.push_back(SplitResponse.Answers.back());
                RetResponse.Header.AnswCount += 1;
                RetResponse.Header.QuesCount += 1;

                // if(RetResponse.Answers.back().DomainEncoding.WasCompress) {
                //     RetResponse.Answers.back().DomainEncoding.SetAddPos(sum_pos);
                //     cout << "sumamos" << endl;
                // }
                //
                // sum_pos += SplitResponse.Questions.back().Len;
                // sum_pos += SplitResponse.Answers.back().Len;

            }

            return RetResponse;
        }

        ~DnsServer() {
            close(LocalSocket);
        }
        void Stop() {
            ShouldStop = true;
        }
};
