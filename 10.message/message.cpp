#include "message.h"
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

Message * Message_New(unsigned short const type,
                    unsigned short const cmd,
                    unsigned short const index,
                    unsigned short const total,
                    const char * payload,
                    unsigned int const length)
{
    Message * ret {static_cast<Message *>(malloc(sizeof(Message) + length))};

    if (ret){
        ret->type = type;
        ret->cmd = cmd;
        ret->index = index;
        ret->total = total;
        ret->length = length;

        if (payload){
            memcpy(ret + 1,payload,length);
            /* (ret+1) 是跳到柔性数组指针*/
        }
    }

    return ret;
}

int Message_Size(const Message *m)
{
    // int ret {};
    // if (m){
    //     ret = sizeof(Message) + m->length;
    // }
    // return ret;

    return m ? (sizeof(Message) + m->length) : 0;
}

Message* Message_N2H(Message *m)
{
    if (m){

        m->type = ntohs(m->type);
        m->cmd = ntohs(m->cmd);
        m->index = ntohs(m->index);
        m->total = ntohs(m->total);
        m->length = ntohl(m->length);
    }

    return m;
}

Message* Message_H2N(Message *m)
{
    if (m){

        m->type = htons(m->type);
        m->cmd = htons(m->cmd);
        m->index = htons(m->index);
        m->total = htons(m->total);
        m->length = htonl(m->length);
    }

    return m;
}
