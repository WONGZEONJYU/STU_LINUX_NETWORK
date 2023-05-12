#include <cstring>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include "list.h"
#include "addr_mgr.h"

#define CMD_SIZE  48
#define IP_SIZE   16

struct SvrAddr
{
    struct list_head head;
    char cmd[CMD_SIZE];
    char ip[IP_SIZE];
};

static LIST_HEAD(g_svrList);

int AddrMgr_Add(const char* cmd, const char* addr)
{
    int ret {0};

    if( cmd && addr )
    {
        char* ip {AddrMgr_Find(cmd)};

        if( ip ){
            ret = !!strcpy(ip, addr);
        }
        else{

            SvrAddr* sa = reinterpret_cast<SvrAddr * >(malloc(sizeof(SvrAddr)));

            if( ret = (!!sa) ){

                strncpy(sa->cmd, cmd, CMD_SIZE);
                strncpy(sa->ip, addr, IP_SIZE);

                sa->cmd[CMD_SIZE-1] = 0;
                sa->ip[IP_SIZE-1] = 0;

                list_add(reinterpret_cast<list_head*>(sa), &g_svrList);
            }
        }
    }

    return ret;
}

char* AddrMgr_Find(const char* cmd)
{
    char* ret {};

    if( cmd ){

        list_head* pos {};
        
        list_for_each(pos, &g_svrList) {

            SvrAddr* c {reinterpret_cast<SvrAddr*>(pos)};

            if( strcmp(cmd, c->cmd) == 0 ){
                ret = c->ip;
                break;
            }
        }
    }

    return ret;
}

void AddrMgr_Remove(const char* cmd)
{
    char* ip {AddrMgr_Find(cmd)};
    
    if( ip ){

        SvrAddr* sa {container_of(ip, SvrAddr, ip)};

        list_del(reinterpret_cast<list_head*>(sa));

        free(sa);
    }
}

void AddrMgr_Clear()
{
    while( !list_empty(&g_svrList) )
    {
        list_head* sa {g_svrList.next};

        list_del(sa);
        
        free(sa);
    }
}

