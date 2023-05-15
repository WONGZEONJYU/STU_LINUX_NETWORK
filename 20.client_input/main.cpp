#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <unistd.h>
#include "utility.h"
#include "msg_def.h"

using namespace std;

#define BUF_SIZE   64
#define DIM(a)     sizeof(a)/sizeof(*a)

static char * Query_Handler(const char * cmd)
{
    cout << "Query_Handler :" << cmd << endl;
    return nullptr;
}

static char * Touch_Handler(const char * cmd)
{
    cout << "Touch_Handler :" << cmd << endl;
    return nullptr;
}

struct Handler
{
    const char * cmd;
    char *(*handler)(const char *);
};

static Handler g_handler[] {
    {"query",Query_Handler},
    {"touch",Touch_Handler},
};

int main(int argc, char const *argv[])
{
    cout << "test main " << endl;

    char line[BUF_SIZE]{};

    char ** arg {Malloc2d<char>(2, BUF_SIZE)};

    while (arg){

        cout << "Input >>> ";

        fgets(line,sizeof(line) - 1,stdin);

        line[strlen(line) - 1] = 0;

        *arg[0] = 0;
        *arg[1] = 0;

        if (*line){

            int r{ DivideByChar(line, ' ', arg, 2,BUF_SIZE) };

            for (size_t i {}; ((i < DIM(g_handler)) && r > 0); i++){

                if (strcmp(g_handler[i].cmd,arg[0]) == 0){
                    g_handler[i].handler(arg[1]);
                    break;
                }
                
            }
            
            // std::cout << "arg[0] = " << arg[0] << std::endl;
            // std::cout << "arg[1] = " << arg[1] << std::endl;
        }
    }

    Free2d(arg);

    return 0;
}
