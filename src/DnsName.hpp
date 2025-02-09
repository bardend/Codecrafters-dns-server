#include <iostream>
#include <cstdint>
#include <vector>
#include <cstring>  // Para memcpy
#include <string>
#include <arpa/inet.h>  // Para htons()
#include <functional>
#include <netinet/in.h>
#include <string>

                        //
using namespace std;

const uint8_t NoCompress = 0x00;
struct StructDomain {
    vector<string>names;
    vector<uint8_t>pointers;
    StructDomain(){}
    void Add(string name, uint8_t pointer = NoCompress) {
        names.push_back(name);
        pointers.push_back(pointer);
    }
};

class CompressStrategy {
    public :
        virtual ~CompressStrategy() = default;
        virtual vector<uint8_t> GetBytes(StructDomain Domains, int extra) = 0;
};

void AddName(vector<uint8_t>&s, string x) {
    s.push_back((int)x.size());
    for(int ii = 0; ii < (int)x.size(); ii++)
        s.push_back(x[ii]);
}

class Uncompress : public CompressStrategy {
    public :
        vector<uint8_t>GetBytes(StructDomain Domains, int extra) override {
            vector<uint8_t>RetBytes;
            vector<string> names = Domains.names;
            for(auto cd: names) 
                AddName(RetBytes, cd);

            RetBytes.push_back(0);
            return RetBytes;
        }
 };

const uint8_t FlagCompress = 0xC0; // MEJORAR

class OnlyParse : public CompressStrategy {
    public :
        vector<uint8_t>GetBytes(StructDomain Domains, int extra) override {
            vector<uint8_t>RetBytes;
            vector<string> names = Domains.names; vector<uint8_t> pointers = Domains.pointers;
            bool IsCompress = false;
            for(int i = 0; i < (int)names.size(); i++) {
                if(pointers[i] == NoCompress)  { //This is normal
                    AddName(RetBytes, names[i]);
                }
                else  { // This is compression and final is here for domain names
                    RetBytes.push_back(FlagCompress);
                    RetBytes.push_back(pointers[i]+extra);
                    IsCompress = true;
                    break; // Ensure 0xC0 pointer is the final of the shadow(Question or Answer)
                }
            }
            if(!IsCompress) 
                RetBytes.push_back(0);
            return RetBytes;
        }
};

        
class DnsName {
    private:
        StructDomain Collector;
        CompressStrategy* strategy;
        int extra = 0;
    public:
        bool WasCompress = 0;
        int Len;
        DnsName() {}
        DnsName(const vector<uint8_t> &buffer, int pos, CompressStrategy* strategy)
                :strategy(strategy){

            uint8_t poin;

            //Two pointers to linear complexity O(len(buffer))
            cout << "The position init Name: " << (int)pos << endl;

            auto GetDomain = [&](int &pos, int IsCompress, auto&& GetDomain) -> void  {
                while(buffer[pos] != 0) {

                    if(!IsCompress) { //can be compress

                        if((int)(buffer[pos] & 0b11000000) == 192) {
                            WasCompress = 1;
                            int pointer = (int)(((buffer[pos] & 0b00111111) << 8) | buffer[pos+1]);
                            poin = pointer;
                            GetDomain(pointer, 1, GetDomain);
                            pos += 2;
                            return;
                        }
                    }

                    string name = "";
                    int len = (int)buffer[pos];
                    while(len--) {
                        pos += 1;
                        name += (char)(buffer[pos]);
                    }

                    if(!IsCompress)
                        Collector.Add(name); //not pointer
                    else 
                        Collector.Add(name, poin);
                    pos += 1;
                }
                pos += 1;
                return;
            };
           
            int i = pos;
            GetDomain(i, 0, GetDomain);

            Len = i - pos;
        }

        vector<uint8_t> GetBytes() {
            return strategy->GetBytes(Collector, extra);
        }

        void SetAddPos(int xtra) {
            extra = xtra;
        }
};
