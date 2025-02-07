#include <iostream>
#include <cstdint>
#include <vector>
#include <cstring>  // Para memcpy
#include <string>
#include <arpa/inet.h>  // Para htons()
#include <functional>
                        //
using namespace std;


class GetBytesStrategy {
    public :
        virtual ~GetBytesStrategy() = default;
        virtual vector<uint8_t> GetBytes(vector<string> Domains) = 0;
        virtual vector<uint8_t> GetBytes(uint8_t pointer) = 0;
};

class GetMockBytes : public GetBytesStrategy {
    public :
        vector<uint8_t>GetBytes(vector<string> Domains) override {
            vector<uint8_t>RetBytes;
            for(auto cd: Domains) {
                RetBytes.push_back((int)cd.size());
                for(int ii = 0; ii < (int)cd.size(); ii++) 
                    RetBytes.push_back(cd[ii]);
            }
            RetBytes.push_back(0);
            return RetBytes;
        }
        vector<uint8_t> GetBytes(uint8_t pointer) override { return vector<uint8_t>();}

};
const uint8_t FlagCompress = 0xC0; // MEJORAR
                                
class GetServerBytes: public GetBytesStrategy {
    public :
        vector<uint8_t> GetBytes(vector<string> Domains) override {return vector<uint8_t>();}

        vector<uint8_t>GetBytes(uint8_t pointer) override {
            vector<uint8_t>RetBytes;
            RetBytes.push_back(FlagCompress);
            RetBytes.push_back(pointer);
            return RetBytes;
        }

};

class DnsName {
    private:
        vector<string>Domains;
        GetBytesStrategy* strategy;
        bool Compress = true;
        uint8_t poin;

    public:

        int Len;
        DnsName() {}
        DnsName(const vector<uint8_t> &buffer, int pos, GetBytesStrategy* strategy)
                :strategy(strategy){

            //Two pointers to linear complexity O(len(buffer))
            cout << "The position init Name: " << (int)pos << endl;

            auto GetDomain = [&](int &pos, int IsCompress, auto&& GetDomain) -> void  {
                while(buffer[pos] != 0) {

                    if(!IsCompress) { //can be compress

                        if((int)(buffer[pos] & 0b11000000) == 192) {
                            int pointer = (int)(((buffer[pos] & 0b00111111) << 8) | buffer[pos+1]);
                            pointer -= SizeHeader;
                            GetDomain(pointer, 1, GetDomain);
                            pos += 2;
                            return;
                        }
                    }

                    string domain = "";
                    int len = (int)buffer[pos];
                    while(len--) {
                        pos += 1;
                        domain += (char)(buffer[pos]);
                    }
                    Domains.push_back(domain);
                    pos += 1;
                }
                pos += 1;
                return;
            };
           
            int i = pos;

            if((int)(buffer[i] & 0b11000000) == 192) { //compression
                cout << "Compression:" << endl;
                int pointer = (int)(((buffer[i] & 0b00111111) << 8) | buffer[i+1]);
                pointer -= SizeHeader;
                poin = buffer[i+1];
                Compress = false;
                GetDomain(pointer, 1, GetDomain);
                i += 2;
            }
            else  // un-compression
                GetDomain(i, 0, GetDomain);

            Len = i - pos;

            cout << "**********************" << endl;
            for(auto cd: Domains) 
                cout << cd << endl;
            cout << endl;
        }

        vector<uint8_t> GetBytes() {
            if(Compress) 
                return strategy->GetBytes(Domains);
            return strategy->GetBytes(poin);
        }
};
