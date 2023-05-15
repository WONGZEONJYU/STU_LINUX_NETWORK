#include <string.h>
#include <cstring>
#include <cstring>
#include <cstdlib>
#include "utility.h"

void Free2d(void* p)
{
    void** pp = reinterpret_cast<void**>(p);

    if( pp && *pp ){
        free(*pp);
    }

    free(pp);
}

char * FormatByChar(const char* src, char c)  // O(n)
{
    int len (src ? strlen(src) : 0);

    char* ret {len ? reinterpret_cast<char *>(malloc(len + 1)) : nullptr};

    if( ret )
    {
        int i {},j{};
        bool flag {};
        
        while( (i < len) && (src[i] == c) ) {
            ++i;
        }

        while( i < len ){

            if( src[i] != c ){

                ret[j++] = src[i];
                flag = false;
            }else{

                if( !flag ){
                    ret[j++] = src[i];
                    flag = true;
                }
            }

            i++;
        }

        if( flag ) {
            j--;
        }

        ret[j] = 0;
    }

    return ret;
}

int DivideByChar(const char* line, char c, char** argv, int row, int col)
{
    int ret {};

    if( line && argv ){

        char* buf { FormatByChar(line, c) };

        int len (buf ? strlen(buf) : 0);

        if( len ){

            buf[len] = c;

            for(int i{},j{}; ( i <= len ) && ( ret < row ); i++) {

                if( buf[i] == c ) {

                    int k {(i-j < col) ? i-j : col};

                    strncpy(argv[ret], buf+j, k);
                    
                    argv[ret][(k < col) ? k : (k-1)] = 0;

                    j = i + 1;

                    ret++;
                }
            }

            free(buf);
        } 
    }
    
    return ret;
}

