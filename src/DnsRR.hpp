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
        uint16_t Type;
        uint16_t Class;
        int TTL;
        uint16_t Length;
        vector<uint8_t>Data;
        DnsQuestion SolveQuestion;
        DnsName DomainEncoding;
        bool IsMock = false;

    public:

        int Len;
        DnsRR(DnsQuestion  q)
              : SolveQuestion(q), DomainEncoding() {
            IsMock = true;
            TTL = 60;
            Length = 4;
            Data.resize(Length);
            for(int i = 0; i < Length; i++)
                Data[i] = 0x08;
        }

        GetBytesStrategy* GetBehavior(const vector<uint8_t> &buffer, int pos) {
               return (buffer[pos] == FlagCompress ? static_cast<GetBytesStrategy*>(new GetServerBytes) : 
                                                     static_cast<GetBytesStrategy*>(new GetMockBytes));
        }

        DnsRR(const vector<uint8_t> &buffer, int pos)
              :DomainEncoding(buffer, pos, GetBehavior(buffer, pos)), SolveQuestion() {

            int CurrentPos = pos + DomainEncoding.Len;
            Type = (uint16_t)(buffer[CurrentPos] << 8 | buffer[CurrentPos + 1]);
            Class = (uint16_t)(buffer[CurrentPos + 2]<< 8 | buffer[CurrentPos + 3]);
            TTL = (buffer[CurrentPos + 4] << 24 | buffer[CurrentPos + 5] << 16 |
                   buffer[CurrentPos + 6] << 8  | buffer[CurrentPos + 7]);
            Length = (uint16_t)(buffer[CurrentPos + 8] << 8 | buffer[CurrentPos + 9]);

            Data.resize(Length);
            for(int i = 0; i < (int)Length; i++) 
                Data[i] = buffer[CurrentPos + 10 + i];
        }

        vector<uint8_t> GetBytes() {
            vector<uint8_t>RetBytes;

            if(IsMock)
                WriteToNetwork(RetBytes, SolveQuestion.GetBytes());

            else {
                WriteToNetwork(RetBytes, DomainEncoding.GetBytes());
                WriteToNetwork(RetBytes, Type);
                WriteToNetwork(RetBytes, Class);
            }

            WriteToNetwork(RetBytes, TTL);
            WriteToNetwork(RetBytes, Length);
            WriteToNetwork(RetBytes, Data);
            return RetBytes;
        }
};
