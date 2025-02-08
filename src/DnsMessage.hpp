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

            auto QuestionCount = Header.QuesCount;

            int CurrentPos = 12;
            cout << "Tenemos cuantas Questions : " << QuestionCount << endl;
            while(QuestionCount--) {
                auto Q = DnsQuestion(buffer, CurrentPos);
                Questions.push_back(Q);
                CurrentPos += (int)Q.Len;
            }

            if(IncludeResponse)
                ParseAnswer(CurrentPos);
        }
        
        void ParseAnswer(int SizeBeforeAnswer) {

            auto AnswerCount = Header.AnswCount;
            int CurrentPos = SizeBeforeAnswer;


            cout << "La posicion a iniciar " << CurrentPos << endl;
            cout << hex <<(int)buffer[CurrentPos] << endl;
            cout << "El numero de AnswerCount : " << AnswerCount << endl;
            while(AnswerCount--) {
                auto A = DnsRR(buffer, CurrentPos);
                Answers.push_back(A);
                CurrentPos += A.Len;
            }
        }

       vector<uint8_t> GetBytes() {
           vector<uint8_t> RetBytes = Header.GetBytes();

           for(auto Question : Questions)
               WriteToNetwork(RetBytes, Question.GetBytes());

           for(auto Answer : Answers)
               WriteToNetwork(RetBytes, Answer.GetBytes());

           return RetBytes;
       }
};
