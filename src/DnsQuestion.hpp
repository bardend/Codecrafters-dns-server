#ifndef DNS_QUESTION_HPP
#define DNS_QUESTION_HPP

#include <cstdint>
#include <vector>
#include <cstring>  // Para memcpy
#include <arpa/inet.h>  // Para htons()
#include <array>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


#include "DnsName.hpp"
#include "NetworkUtils.hpp"


using namespace std;

class DnsQuestion {
    private:
        DnsName DomainEncoding;
        
    public:
        int Len;
        uint16_t Type;
        uint16_t Class;

        DnsQuestion(const vector<uint8_t> &buffer, int pos) 
                    : DomainEncoding(buffer, pos, new Uncompress) {
            //This call at construct DomainEncoding with (buffer and pos) first
            int CurrentPos = pos + DomainEncoding.Len;

            Type = (uint16_t)(buffer[CurrentPos] << 8 | buffer[CurrentPos + 1]);
            Class = (uint16_t)(buffer[CurrentPos + 2]<< 8 | buffer[CurrentPos + 3]);
            Len = DomainEncoding.Len + 4;
        }
        DnsQuestion(){}

        vector<uint8_t> GetBytes() {
            vector<uint8_t>RetBytes;

            WriteToNetwork(RetBytes, DomainEncoding.GetBytes());
            WriteToNetwork(RetBytes, Type);
            WriteToNetwork(RetBytes, Class);
            return RetBytes;
        }
};
        
#endif // DNS_QUESTION_HPP
