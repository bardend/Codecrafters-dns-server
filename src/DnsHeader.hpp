#ifndef DNS_HEADER_HPP
#define DNS_HEADER_HPP

#include <cstdint>
#include <iostream>
#include <cstring>  // Para memcpy
#include <vector>
#include <arpa/inet.h>  // Para htons() y htonl()


const int SizeHeader = 12;

using namespace std;
class DnsHeader {
private:
    uint16_t ID;           // Identificador único
    uint8_t QR;            // Query/Response flag
    uint8_t OpCode;        // Código de operación
    uint8_t AuthAns;       // Autoritative Answer flag
    uint8_t Trun;          // Truncation flag
    uint8_t RecurDes;      // Recursion Desired flag
    uint8_t RecurAva;      // Recursion Available flag
    uint8_t Reserved;      // Reservado
    uint8_t RespCode;      // Código de respuesta

public:
    uint16_t QuesCount;    // Número de preguntas
    uint16_t AnswCount;    // Número de respuestas
    uint16_t AuthCount;    // Número de registros de autoridad
    uint16_t AddiCount;    // Número de registros adicionales

    // Constructor por defecto
    DnsHeader() 
        : ID(0), QR(0), OpCode(0), AuthAns(0), Trun(0), RecurDes(0), RecurAva(0),
          Reserved(0), RespCode(0), QuesCount(0), AnswCount(0), AuthCount(0), AddiCount(0) {}



    DnsHeader(const uint8_t* buffer) {

        ID = (uint16_t)(buffer[0] << 8 | buffer[1]);

        QR = (buffer[2] >> 7) & 1 ;
        OpCode = (uint8_t)((buffer[2] & 0b01111000) >> 3);
        AuthAns = (uint8_t)((buffer[2] >> 2) & 1);
        Trun = (uint8_t)((buffer[2] >> 1) & 1);
        RecurDes = (uint8_t)(buffer[2] & 1);

        RecurAva = (uint8_t)((buffer[3] >> 7) & 1);
        Reserved = (uint8_t)((buffer[3] & 0b01110000) >> 4);
        RespCode = (uint8_t)(buffer[3] & 0b00001111);

        QuesCount = (uint16_t)(buffer[4] << 8 | buffer[5]);
        AnswCount = (uint16_t)(buffer[6] << 8 | buffer[7]);
        AuthCount = (uint16_t)(buffer[8] << 8 | buffer[9]);
        AddiCount = (uint16_t)(buffer[10] << 8 | buffer[11]);
    }

    vector<uint8_t> GetBytes() {
        std::vector<uint8_t> buffer(SizeHeader);  

        uint16_t id_net = htons(ID); 
        memcpy(buffer.data(), &id_net, sizeof(id_net));

        uint8_t SecondByte = (QR << 7) | (OpCode << 3) | (AuthAns << 2) | (Trun << 1) | (RecurDes);
        uint8_t ThirdByte = (RecurAva << 7) | (Reserved << 4) | (RespCode);
        buffer[2] = SecondByte;
        buffer[3] = ThirdByte;

        uint16_t ques_count_net = htons(QuesCount);
        uint16_t answ_count_net = htons(AnswCount);
        uint16_t auth_count_net = htons(AuthCount);
        uint16_t addi_count_net = htons(AddiCount);

        memcpy(buffer.data() + 4, &ques_count_net, sizeof(ques_count_net));
        memcpy(buffer.data() + 6, &answ_count_net, sizeof(answ_count_net));
        memcpy(buffer.data() + 8, &auth_count_net, sizeof(auth_count_net));
        memcpy(buffer.data() + 10, &addi_count_net, sizeof(addi_count_net));

        return buffer;  // Devolvemos el buffer como un vector
    }


    void SetID(uint16_t id) { ID = id; }
    void SetQR(uint8_t qr) { QR = qr; }
    void SetOpCode(uint8_t opcode) { OpCode = opcode; }
    void SetAuthAns(uint8_t authAns) { AuthAns = authAns; }
    void SetTrun(uint8_t trun) { Trun = trun; }
    void SetRecurDes(uint8_t recurDes) { RecurDes = recurDes; }
    void SetRecurAva(uint8_t recurAva) { RecurAva = recurAva; }
    void SetReserved(uint8_t reserved) { Reserved = reserved; }
    void SetRespCode(uint8_t respCode) { RespCode = respCode; }
    void SetQuesCount(uint16_t quesCount) { QuesCount = quesCount; }
    void SetAnswCount(uint16_t answCount) { AnswCount = answCount; }
    void SetAuthCount(uint16_t authCount) { AuthCount = authCount; }
    void SetAddiCount(uint16_t addiCount) { AddiCount = addiCount; }

};

#endif // DNS_HEADER_HPP

