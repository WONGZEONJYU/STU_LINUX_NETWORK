#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "msg_parser.h"

using namespace std;

int main(int argc, char* argv[])
{
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
    cout << "===========================================================\n";
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

    return 0;
}
