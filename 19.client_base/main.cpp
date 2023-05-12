#include <cstdio>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <string>
#include "utility.h"
#include "addr_mgr.h"

int main()
{
    // AddrMgr_Add("wongzeonjyu19970", "1.1.1.1");
    // printf("%s\r\n",AddrMgr_Find("wongzeonjyu19970"));

    // AddrMgr_Add("wongzeonjyu19970","2.2.2.2");
    // printf("%s\r\n",AddrMgr_Find("wongzeonjyu19970"));

    // AddrMgr_Clear();

    char ** p {Malloc2d<char>(3, 7)};

    const char * src {"    abc    de    fg   "};

    int r {DivideByChar(src,' ',p,3,7)};

    for (size_t i = 0; i < r; i++){

        std::cout << "p[" << i << "] = " << p[i] << std::endl;
    }

    Free2d(p);

    return 0;
}
