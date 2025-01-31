#include <cstdint>
#include <vector>
#include <cstring>  // Para memcpy
#include <arpa/inet.h>  // Para htons()
#include <array>
#include "DnsHeader.hpp"
#include "DnsQuestion.hpp"

using namespace std;

class DnsMessage {
    private:
        DnsHeader Header;
        vector<DnsQuestion> Questions;

    public:

        DnsMessage(const uint8_t* buffer)
                   : Header(buffer) {

            Header.QR = 1;
            const uint8_t* HeaderlessBytes = buffer + SizeHeader;
            
            auto QuestionCount = Header.QuesCount;

            int CurrentPos = 0;
            while(QuestionCount--) {
                auto Q = DnsQuestion(HeaderlessBytes, CurrentPos);
                Questions.push_back(Q);
                CurrentPos += Q.Len;
            }
        }

       vector<uint8_t> GetBytes() {
           vector<uint8_t> RetBytes = Header.GetBytes();

           for(auto Question : Questions) {
               auto Q = Question.GetBytes();
               RetBytes.insert(RetBytes.end(), Q.begin(), Q.end());
           }
           return RetBytes;
       }
};
        

