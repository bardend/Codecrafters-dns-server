#include <cstdint>
#include <vector>
#include <cstring>  // Para memcpy
#include <arpa/inet.h>  // Para htons()
#include <array>

//#include "DnsName.hpp" Como tu padre ya lo heredo tu lo conoces ????????
#include "NetworkUtils.hpp"

using namespace std;

class DnsRR {
    private:
        DnsName DomainEncoding;
        uint16_t Type;
        uint16_t Class;
        int TTL;
        uint16_t Length;
        vector<uint8_t>Data;

    public:
        int Len;
        DnsRR(const uint8_t* buffer, int pos) 
              :DomainEncoding(buffer, pos) {

            int CurrentPos = pos + DomainEncoding.Len;
            Type = (uint16_t)(buffer[CurrentPos] << 8 | buffer[CurrentPos+1]);
            Class = (uint16_t)(buffer[CurrentPos + 2] << 8 | buffer[CurrentPos + 3]);
            TTL = buffer[CurrentPos + 4] << 24 | buffer[CurrentPos + 5] << 16 |
                  buffer[CurrentPos + 6] << 8 | buffer[CurrentPos + 7];

            // Length = (uint16_t)(buffer[CurrentPos + 8] << 8 | buffer[CurrentPos + 9]);
            // Data.resize(Length);
            // CurrentPos += 10;
            //
            // for(int i = 0; i < Length; i++) {
            //     Data[i] = buffer[CurrentPos++];
            // }
            // // Set dafault values
            Length = 4;
            Data.resize(Length);
            CurrentPos += 10;
            for(int i = 0; i < Length; i++)
                Data[i] = 0x08;
            CurrentPos += Length;

            Len = CurrentPos - pos;

        }

        vector<uint8_t> GetBytes() {
            vector<uint8_t>RetBytes;

            WriteToNetwork(RetBytes, DomainEncoding.GetBytes());
            WriteToNetwork(RetBytes, Type);
            WriteToNetwork(RetBytes, Class);
            WriteToNetwork(RetBytes, TTL);
            WriteToNetwork(RetBytes, Length);
            WriteToNetwork(RetBytes, Data);
            return RetBytes;
        }
};
