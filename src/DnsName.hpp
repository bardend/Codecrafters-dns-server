#include <cstdint>
#include <vector>
#include <cstring>  // Para memcpy
#include <string>
#include <arpa/inet.h>  // Para htons()
                        //
using namespace std;

class DnsName {
    private:
        vector<string>Domains;

    public:

        int Len;
        DnsName(const uint8_t* buffer, int pos) {

            //Two pointers to linear complexity O(len(buffer))
            auto GetDomain = [&](int &pos) {
                string domain = "";
                int len = (int)buffer[pos];

                while(len--) {
                    pos += 1;
                    domain += (char)(buffer[pos]);
                }
                pos += 1;
            };

            int i = pos;
            while(buffer[i] != 0) {

                if((int)(buffer[i] & 0b11000000) == 192) { //compression
                    int pointer = (int)((buffer[i] & 0b00111111) << 8 | buffer[i+1]);
                    GetDomain(pointer);
                    i += 2;
                }
                else  // un-compression
                    GetDomain(i);
            }
            Len = i - pos + 1;
        }

        vector<uint8_t> GetBytes() {
            vector<uint8_t>RetBytes(Len);
            int i = 0;
            for(auto cd: Domains) {
                RetBytes[i++] = (int)cd.size();
                for(int ii = 0; ii < (int)cd.size(); ii++) 
                    RetBytes[i++] = cd[ii];
            }
            RetBytes[i] = 0; // El final perro
            return RetBytes;
        }
};
