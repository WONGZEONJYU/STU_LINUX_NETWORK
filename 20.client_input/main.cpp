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

    char buf[BUF_SIZE]{};

    char ** arg {Malloc2d<char>(2, BUF_SIZE)};

    while (arg){

        cout << "Input >>> ";

        cin >> buf;

        int r{ DivideByChar(buf, ' ', arg, 2,BUF_SIZE) };

        cout << line[0] << endl;

    }

    Free2d(line);

    return 0;
}
