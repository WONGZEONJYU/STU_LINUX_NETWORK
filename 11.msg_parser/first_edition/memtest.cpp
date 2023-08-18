#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "msg_parser.h"

using namespace std;

int main(int argc, char* argv[])
{
#if 0
    {
        MParser* p {MParser_New()};
        char buf[] {0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04};
        char data[] {0x11, 0x12, 0x13, 0x14};
        Message* m { MParser_ReadMem(p, reinterpret_cast<uint8_t*>(buf), sizeof(buf))};

        if( !m ){
            cout << "parse again...\n";
            // getchar();
            m = MParser_ReadMem(p, reinterpret_cast<uint8_t*>(data), sizeof(data));
        }

        cout << "m = " << m << '\n';

        if( m ){
            cout << "type = " << m->type << '\n';
            cout << "cmd = " << m->cmd << '\n';
            cout << "index = " << m->index << '\n';
            cout << "total = " << m->total << '\n';
            cout << "length = " << m->length << '\n';
            for(int i{}; i<m->length; i++){
                cout << "0x" << hex << static_cast<int>(m->payload[i]) << ' ';
            }
            cout << '\n';
            free(m);
        }

        MParser_Del(p);
    }
#endif
    cout << "===========================================================\n";
#if 0

    {
        MParser* p {MParser_New()};
        char buf[] {0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05,0x21,0x22,0x23,0x24,0x25};
        Message* m { MParser_ReadMem(p, reinterpret_cast<uint8_t*>(buf), sizeof(buf))};
        cout << "m = " << m << '\n';

        if( m ){
            cout << "type = " << m->type << '\n';
            cout << "cmd = " << m->cmd << '\n';
            cout << "index = " << m->index << '\n';
            cout << "total = " << m->total << '\n';
            cout << "length = " << m->length << '\n';
            for(int i{}; i<m->length; i++){
                cout << "0x" << hex << static_cast<int>(m->payload[i]) << ' ';
            }
            cout << '\n';
            free(m);
        }

        MParser_Del(p);
    }

#endif
    cout << "===========================================================\n";
#if 1

    {

        MParser * p {MParser_New()};

        char buf[] { 0x00,0x01,0x00,0x02,0x00};
        char another[] {0x03,0x00,0x04,0x00,0x00,0x00,0x04};
        char data[] {0x31,0x32,0x33,0x34};

        Message * m {MParser_ReadMem(p,reinterpret_cast<uint8_t *>(buf),sizeof(buf))};

        cout << "m = " << m << '\n';

        if (!m){
            cout << "parser again...\n";
            m = MParser_ReadMem(p,reinterpret_cast<uint8_t * >(another),sizeof(another));
        }

        cout << "m = " << m << '\n';

        if (!m){
            cout << "parser again again...\n";
            m = MParser_ReadMem(p,reinterpret_cast<uint8_t * >(data),sizeof(data));
        }

        cout << "m = " << m << '\n';

        if (m){
            
            cout << "type = " << m->type << 
                    "\ncmd = " << m->cmd << 
                    "\nindex = " << m->index << 
                    "\ntotal = " << m->total << 
                    "\nlength = " << m->length << '\n';

            for (int i {}; i < m->length; i++) {
                cout << "0x" << hex << static_cast<int>(m->payload[i]) << '\n';
            }

            cout << '\n';

            free(m);
        }

        MParser_Del(p);


    }

#endif

    return 0;
}
