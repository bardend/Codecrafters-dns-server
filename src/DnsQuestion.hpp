#ifndef DNS_QUESTION_HPP
#define DNS_QUESTION_HPP

#include <cstdint>
#include <vector>
#include <cstring>  // Para memcpy
#include <arpa/inet.h>  // Para htons()
#include <array>

#include "DnsName.hpp"


class DnsQuestion {
    private:
        DnsName DomainEncoding;
        uint16_t Type;
        uint16_t Class;

    public:
        int Len;
        DnsQuestion(const uint8_t* buffer, int pos) 
                    : DomainEncoding(buffer, pos) {
            //This call at construct DomainEncoding with (buffer and pos) first
            int CurrentPos = pos + DomainEncoding.Len;

            Type = (uint16_t)(buffer[CurrentPos] << 8 | buffer[CurrentPos + 1]);
            Class = (uint16_t)(buffer[CurrentPos + 2]<< 8 | buffer[CurrentPos + 3]);
            Len = DomainEncoding.Len + 4;
        }

        vector<uint8_t> GetBytes() {
            vector<uint8_t>RetBytes(Len);
            vector<uint8_t>NamesEncoding = DomainEncoding.GetBytes();
            copy(NamesEncoding.begin(), NamesEncoding.end(), RetBytes.begin());

            int CurrentPos = DomainEncoding.Len;

            uint16_t Type_net = htons(Type);
            memcpy(RetBytes.data() + CurrentPos, &Type_net, sizeof(Type_net));

            uint16_t Class_net = htons(Class);
            memcpy(RetBytes.data() + CurrentPos + 2, &Class_net, sizeof(Class_net));

            return RetBytes;
        }
};
        
#endif // DNS_QUESTION_HPP
