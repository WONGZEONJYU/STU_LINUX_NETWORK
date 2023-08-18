#include <iostream>
#include <cstdio>
#include "msg_parser.h"
#include "../msg_parser.h"

using namespace std;

int main(int argc, char const *argv[])
{
    MParser * p {MParser_New()};

    char buf[] { 0x00,0x01,0x00,0x02,0x00};
    char another[] {0x03,0x00,0x04,0x00,0x00,0x00,0x04};
    char data[] {0x11,0x12,0x13,0x14};

    Message * m = MParser_ReadMem(p,reinterpret_cast<uint8_t *>(buf),sizeof(buf));
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
            printf("0x%02x ",m->payload[i]);
        }

        cout << '\n';

        free(m);
    }

    MParser_Del(p);

    return 0;
}
