#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iomanip>  // Para setw y setfill


using namespace std;

int main() {
    // Crear el socket UDP
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Error creando socket");
        return 1;
    }

    // Configurar el servidor DNS (8.8.8.8:53)
    sockaddr_in forwardServer{};
    forwardServer.sin_family = AF_INET;
    forwardServer.sin_port = htons(53);
    forwardServer.sin_addr.s_addr = inet_addr("8.8.8.8");

    // Establecer timeout para el socket
    struct timeval tv;
    tv.tv_sec = 5;  // 5 segundos de timeout
    tv.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("Error configurando timeout");
        close(sock);
        return 1;
    }

    // Tu query DNS (la mantenemos igual)
    vector<uint8_t> Query = {
        0x96, 0xd3, 0x81, 0x20, 0x00, 0x01, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x0c, 0x63, 0x6f, 0x64, 
        0x65, 0x63, 0x72, 0x61, 0x66, 0x74, 0x65, 0x72, 
        0x73, 0x02, 0x69, 0x6f, 0x00, 0x00, 0x01, 0x00, 
        0x01
    };

    // Enviar la query
    ssize_t bytesSent = sendto(sock, Query.data(), Query.size(), 0,
                              (struct sockaddr*)&forwardServer, sizeof(forwardServer));
    if (bytesSent < 0) {
        perror("Error enviando datos");
        close(sock);
        return 1;
    }

    cout << "Query DNS enviada (" << bytesSent << " bytes):" << endl;
    for(size_t i = 0; i < Query.size(); i++) {
        cout << hex << uppercase << setw(2) << setfill('0') << (int)Query[i] << " ";
    }
    cout << endl;

    // Recibir la respuesta
    vector<uint8_t> buffer(1024);
    sockaddr_in fromAddr{};
    socklen_t fromLen = sizeof(fromAddr);
    
    ssize_t bytesReceived = recvfrom(sock, buffer.data(), buffer.size(), 0,
                                    (struct sockaddr*)&fromAddr, &fromLen);
    
    if (bytesReceived < 0) {
        perror("Error recibiendo datos");
        close(sock);
        return 1;
    }

    cout << "Respuesta DNS recibida (" << bytesReceived << " bytes):" << endl;
    for(int i = 0; i < bytesReceived; i++) {
        cout << hex << uppercase << setw(2) << setfill('0') << (int)buffer[i] << " ";
    }
    cout << endl;

    close(sock);
    return 0;
}
