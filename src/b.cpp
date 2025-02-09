#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iomanip>
#include <errno.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <net/if.h>

using namespace std;

int main() {

    // Crear socket UDP
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Error creando socket");
        return 1;
    }

    // Intentar vincular a una interfaz específica (0.0.0.0)
    sockaddr_in localAddr{};
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = 0;  // Puerto aleatorio
    localAddr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sock, (struct sockaddr*)&localAddr, sizeof(localAddr)) < 0) {
        perror("Error en bind");
        close(sock);
        return 1;
    }

    // Obtener y mostrar el puerto local asignado
    socklen_t len = sizeof(localAddr);
    if (getsockname(sock, (struct sockaddr*)&localAddr, &len) < 0) {
        perror("Error en getsockname");
    } else {
        cout << "\nPuerto local asignado: " << ntohs(localAddr.sin_port) << endl;
    }


    // Configurar Google DNS
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(53);
    serverAddr.sin_addr.s_addr = inet_addr("8.8.8.8");

    // Timeout más largo para debug
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("Error configurando timeout");
    }

    // Query DNS simple (example.com)
    vector<uint8_t> Query = {
        0x02, 0x01,  // ID
        0x01, 0x00,  // Flags
        0x00, 0x01,  // Questions
        0x00, 0x00,  // Answer RRs
        0x00, 0x00,  // Authority RRs
        0x00, 0x00,  // Additional RRs
        0x07, 'e', 'x', 'a', 'm', 'p', 'l', 'e',
        0x03, 'c', 'o', 'm',
        0x00,        // Root label
        0x00, 0x01,  // Type A
        0x00, 0x01   // Class IN
    };

    cout << "\nEnviando query a 8.8.8.8..." << endl;
    
    ssize_t bytesSent = sendto(sock, Query.data(), Query.size(), 0,
                              (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (bytesSent < 0) {
        cout << "Error enviando datos: " << strerror(errno) << endl;
        close(sock);
        return 1;
    }

    cout << "Query enviada exitosamente (" << bytesSent << " bytes)" << endl;

    vector<uint8_t> buffer(1024);
    sockaddr_in fromAddr{};
    socklen_t fromLen = sizeof(fromAddr);
    
    cout << "Esperando respuesta..." << endl;
    
    ssize_t bytesReceived = recvfrom(sock, buffer.data(), buffer.size(), 0,
                                    (struct sockaddr*)&fromAddr, &fromLen);
    
    if (bytesReceived < 0) {
        cout << "Error recibiendo datos: " << strerror(errno) << endl;
    } else {
        for(int i = 0; i < 12; i++) 
            cout << hex << (int)buffer[i] << " ";
        cout << endl;

        cout << "Respuesta recibida de " << inet_ntoa(fromAddr.sin_addr) 
             << ":" << ntohs(fromAddr.sin_port) 
             << " (" << bytesReceived << " bytes)" << endl;
    }

    close(sock);
    return 0;
}
