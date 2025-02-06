#include <cstdint>
#include <vector>
#include <cstring>  // Para memcpy
#include <arpa/inet.h>  // Para htons()
#include <array>

#include "DnsHeader.hpp"
#include "DnsQuestion.hpp"
#include "DnsRR.hpp"
#include "NetworkUtils.hpp"

using namespace std;

class DnsMessage {
    private:
        vector<uint8_t>buffer;

    public:
        DnsHeader Header;
        vector<DnsQuestion> Questions;
        vector<DnsRR> Answers;
        bool IncludeResponse;

        DnsMessage(vector<uint8_t> buffer, bool IncludeResponse)
                   : Header(buffer),
                     buffer(buffer), IncludeResponse(IncludeResponse) {
            ParseQuestion();
        }

        DnsMessage(DnsHeader header)
                   : Header(header) {}

        void ParseQuestion() {

            vector<uint8_t> HeaderlessBytes(buffer.begin() + SizeHeader, buffer.end());
            auto QuestionCount = Header.QuesCount;

            int CurrentPos = 0;
            while(QuestionCount--) {
                auto Q = DnsQuestion(HeaderlessBytes, CurrentPos);
                Questions.push_back(Q);
                CurrentPos += (int)Q.Len;
            }
            if(IncludeResponse)
                ParseAnswer(CurrentPos + SizeHeader);
        }
        
        void ParseAnswer(int SizeBeforeAnswer) {

            auto AnswerCount = Header.AnswCount;
            int CurrentPos = SizeBeforeAnswer;

            while(AnswerCount--) {
                auto A = DnsRR(buffer, CurrentPos);
                Answers.push_back(A);
                CurrentPos += A.Len;
            }
        }

       vector<uint8_t> GetBytes() {
           vector<uint8_t> RetBytes = Header.GetBytes();

           cout << "int SizeQuestion :" << (int)Questions.size() << endl;
           for(auto Question : Questions)
               WriteToNetwork(RetBytes, Question.GetBytes());


           for(auto Answer : Answers)
               WriteToNetwork(RetBytes, Answer.GetBytes());

           return RetBytes;
       }
};
