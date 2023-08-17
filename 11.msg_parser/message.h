#ifndef MESSAGE_H
#define MESSAGE_H

struct Message
{
    unsigned short type,cmd,index,total;
    unsigned int length;
    unsigned char payload[];
};

Message * Message_New(unsigned short type,
                    unsigned short cmd,
                    unsigned short index,
                    unsigned short total,
                    const char * payload,
                    unsigned int length);

int Message_Size(Message *m);
Message *Message_N2H(Message *m);
Message *Message_H2N(Message *m);

#endif // MESSAGE_H
