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
        int TTL;
        uint16_t Length;
        vector<uint8_t>Data;
        DnsQuestion SolveQuestion;

    public:

        int Len;
        DnsRR(DnsQuestion  q)
              : SolveQuestion(q) {
            TTL = 60;
            Length = 4;
            Data.resize(Length);
            for(int i = 0; i < Length; i++)
                Data[i] = 0x08;
        }

        vector<uint8_t> GetBytes() {
            vector<uint8_t>RetBytes;

            WriteToNetwork(RetBytes, SolveQuestion.GetBytes());
            WriteToNetwork(RetBytes, TTL);
            WriteToNetwork(RetBytes, Length);
            WriteToNetwork(RetBytes, Data);
            return RetBytes;
        }
};
