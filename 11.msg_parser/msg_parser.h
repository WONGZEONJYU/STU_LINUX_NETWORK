#ifndef MSG_PARSER_H
#define MSG_PARSER_H

#include "message.h"

using MParser = void;

MParser * MParser_New();
Message * MParser_ReadMem(MParser * parser,unsigned char * mem,unsigned int length);
Message * MParser_ReadFd(MParser * parser,int fd);
void MParser_Reset(MParser * parser);
void MParser_Del(MParser * parser);

#endif //MSG_PARSER_H
