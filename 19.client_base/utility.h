#ifndef UTILITY_H
#define UTILITY_H

#include <cstdlib>

template<typename T> 
static inline T ** Malloc2d(int row, int col)
{
    T ** ret {};

    if( (row > 0) && (col > 0) ){

        ret = reinterpret_cast<T**>(malloc(row * sizeof(T*)));

        T * p {reinterpret_cast<T*>(malloc(row * col * sizeof(T)))};

        if (ret && p){
            
            for (size_t i = 0; i < row; i++){
                
                ret[i] = p + i * col;
            }
        }else{
            free(ret);
            free(p);
        }
    }

    return ret;
}

void Free2d(void* p);
char* FormatByChar(const char* src, char c);
int DivideByChar(const char* line, char c, char** argv, int row, int col);

#endif
