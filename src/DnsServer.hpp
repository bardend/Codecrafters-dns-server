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
        string ipAddress;
        int port;
        bool ShouldStop = false;
        IPEndPoint ForwardServer;

    public:
        DnsServer(const std::string& ip, int serverPort, IPEndPoint forwardServer) 
                  : ipAddress(ip), port(serverPort), ForwardServer(forwardServer) {

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
                cout <<"===============================VAMOS A EMPEZAR============================="<< endl;
                struct sockaddr_in clientAddress;
                socklen_t clientAddrLen = sizeof(clientAddress);

                vector<uint8_t>buffer(512);

                int bytesRead = recvfrom(udpSocket, buffer.data(), buffer.size(), 0, 
                                reinterpret_cast<struct sockaddr*>(&clientAddress), &clientAddrLen);

                if (bytesRead == -1) {
                    std::cerr << "Receive error" << std::endl;
                    continue;
                }
                                                              
               // buffer = {
               //          //Heder
               //          0xBD, 0x1D, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
               //          //Question1
               //          0x03, 0x61, 0x62, 0x63, 0x11, 0x6C, 0x6F, 0x6E, 0x67, 0x61, 0x73, 0x73, 
               //          0x64, 0x6F, 0x6D, 0x61, 0x69, 0x6E, 0x6E, 0x61, 0x6D, 0x65, 0x03, 0x63, 
               //          0x6F, 0x6D, 0x00, 0x00, 0x01, 0x00, 0x01,
               //
               //          //Question2
               //          0x03, 0x64, 0x65, 0x66, 0xC0, 0x10, 0x00, 0x01, 0x00, 0x01
               //      };
               // bytesRead =  53;
               //

                buffer[bytesRead] = 0x00;

                cout << "Puerto before readBytes : " << ForwardServer.Port << endl;

                cout << "Mensajes desde el mi localhost dig" << endl;
               for(int i = 0; i < bytesRead; i++) 
                       cout << hex << (int)buffer[i] << " ";  // Imprime en hexadecimal

               cout << endl;

               cout << "Puerto after readBytes: " << ForwardServer.Port << endl;

               DnsMessage RequestMessage = DnsMessage(buffer, 0);

               vector<uint8_t> response = ProcessRequest(RequestMessage).GetBytes();


               cout << "Envio final al servidor de codecrafeters" << endl;
               for(int i = 0; i < (int)response.size(); i++) {
                   cout << hex << (int)response[i] << " ";
               }
               cout << endl;

               cout << "=========================TERMINAMOS ENVIAR==============================" << endl;

               if (sendto(udpSocket, response.data(), response.size(), 0, 
                   reinterpret_cast<struct sockaddr*>(&clientAddress), sizeof(clientAddress)) == -1) {
                   perror("Failed to send response");
               }
            }
        }


        /* This server respond only one Question */
        DnsMessage ForwardRequest(DnsMessage Request) {

            cout << "Puerto after readBytes: " << ForwardServer.Port << endl;
            int sock = socket(AF_INET, SOCK_DGRAM, 0); // ⟶ new UdpClient()
            sockaddr_in forwardServer{};
            forwardServer.sin_family = AF_INET;
            forwardServer.sin_port = htons(ForwardServer.Port); // Puerto del servidor
            forwardServer.sin_addr.s_addr = inet_addr(ForwardServer.Address.c_str()); //string a const char*

            if (connect(sock, (struct sockaddr*)&forwardServer, sizeof(forwardServer)) < 0) {
                perror("Error en connect");
                close(sock);
            } // Client.Connect(ForwardServer)
              //

            cout << "1 mensaje enviado al servidor; )" << endl;
            vector<uint8_t> Query = Request.GetBytes();
            for(int i = 0; i < (int)Query.size(); i++) 
                cout << hex << (int)Query[i] << " ";  // Imprime en hexadecimal
            cout << endl;
            
            send(sock, Query.data(), Query.size(), 0);

            vector<uint8_t> buffer(1024); // Espacio para recibir datos
            ssize_t bytesReceived = recv(sock, buffer.data(), buffer.size(), 0);

            /*
        buffer = {
        0x9B, 0x6F, 0x81, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x73, 0x74, 0x61, 
        0x63, 0x6B, 0x6F, 0x76, 0x65, 0x72, 0x66, 0x6C, 0x6F, 0x77, 0x03, 0x63, 0x6F, 0x6D, 0x00, 0x00, 
        0x01, 0x00, 0x01, 0x0D, 0x73, 0x74, 0x61, 0x63, 0x6B, 0x6F, 0x76, 0x65, 0x72, 0x66, 0x6C, 0x6F, 
        0x77, 0x03, 0x63, 0x6F, 0x6D, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x0E, 0x10, 0x00, 0x04, 
        0x97, 0x65, 0x81, 0x45
        };
        bytesReceived = 68;
        */

            cout << "Mensaje recivido desde servidor codecrafeters:" << endl;
            for(int i = 0; i < bytesReceived; i++)
                cout << hex << (int)buffer[i] << " ";  // Imprime en hexadecimal
            cout << endl;

            cout << "Vamos a parsear el mensaje de codecrafeters" << endl;
            DnsMessage ResponseGoogle = DnsMessage(buffer, 1);

            cout << "El numero de questions codecrafeters parceado: " << ResponseGoogle.Header.QuesCount << endl;

            vector<uint8_t>xd = ResponseGoogle.GetBytes();
            for(int i = 0; i < (int)xd.size(); i++) 
                cout << hex << (int)xd[i] << " ";

            cout << endl;

            cout << "Terminamos de parsear el mensaje de codecrafeters " << endl;
            return ResponseGoogle;
        }


        DnsMessage ProcessRequest(DnsMessage Request) {

            cout << "El numero de questions essssssssss: " << Request.Header.QuesCount << endl;

            vector<uint8_t>q = Request.GetBytes();
            cout << "Mensaje setteado como Request" << endl;
            for(int i = 0; i < (int)q.size(); i++) 
                cout << hex << (int)q[i] << " ";  // Imprime en hexadecimal
            cout << endl;


            DnsMessage ResponseMessage(Request.Header);
            
            int sum_pos = 0;
            for(auto Q: Request.Questions) {
                // Set the header
                DnsMessage Split(Request.Header);
                Split.Header.QR = 1;
                Split.Header.AnswCount = 0;
                Split.Header.RespCode = (Split.Header.OpCode == 0x00 ? 0x00 : 0x04);
                Split.Header.QuesCount = 1;
                // Set Question
                Split.Questions.push_back(Q);
                // Send server codecrafeters
                cout << "Enviamos al servidor privado" << endl;
                DnsMessage SplitResponse = ForwardRequest(Split);

                ResponseMessage.Questions.push_back(SplitResponse.Questions[0]);
                ResponseMessage.Answers.push_back(SplitResponse.Answers[0]);



                if(ResponseMessage.Answers.back().DomainEncoding.WasCompress)
                    ResponseMessage.Answers.back().DomainEncoding.SetSubtracPos(sum_pos);
                 //fix the position
                sum_pos += SplitResponse.Questions[0].Len;
                sum_pos += SplitResponse.Answers[0].Len;
            }
            return ResponseMessage;
        }

        ~DnsServer() {
            close(udpSocket);
        }
        void Stop() {
            ShouldStop = true;
        }
};
