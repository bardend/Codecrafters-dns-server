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
            int i = pos, j;

            //Two pointers to linear complexity O(len(buffer))
            while(buffer[i] != 0) {
                string domain = "";
                for(j = i + 1; j <= i + (int)buffer[i]; j++) {
                    domain += char(buffer[j]);
                }
                Domains.push_back(domain);
                i = j;
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
