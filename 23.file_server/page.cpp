#include <iostream>
#include <cstdlib>
#include <cstring>
#include "page.h"

static const char* PAGE_FORMAT {
"<!DOCTYPE html>"
"<html>"
"    <head>"
"        <meta charset=\"utf-8\">"
"        <title>回炉重学html</title>"
"    </head>"
"    <body>"
"        <h1>WJZ Http File Server</h1>"
"        <hr/>"
"        <h3>Path: %s</h3>"
"        %s"
"    </body>"
"</html>"
};

static const char* ERR_FORMAT {
"<!DOCTYPE html>"
"<html>"
"    <head>"
"        <meta charset=\"utf-8\">"
"        <title>回炉重学html</title>"
"    </head>"
"    <body>"
"        <h1>WJZ Http File Server</h1>"
"        <hr/>"
"        <h2>00PS!!!</h2>"
"        <h3>Path: \'%s\' is NOT available...</h3>"
"    </body>"
"</html>"
};

static const char* TABLE_BEGIN {"<table border = \"1\" width = \"100%\">"};

static const char* TABLE_TITLE {
"  <tr>"
"    <th>File Name</th><th>File Type</th><th>File Size</th><th>Modify Time</th>"
"  </tr>"
};

static const char* ROW_FORMAT {
"<tr>"
    "   <td><a href=\"%s\">%s</a></td><td>%s</td><td>%s</td><td>%s</td>"
"</tr>"
};

static const char* TABLE_END {
"   </tr>"
"</table>"
};

char* ToPageString(const char* path,const char* ts)
{
    char* ret {};

    if ( path && ts && (ret = reinterpret_cast<char*>(malloc(strlen(PAGE_FORMAT) + 
        strlen(path) + strlen(ts) + 1))) ){
        
        sprintf(ret,PAGE_FORMAT,path,ts);
    }

    return ret;
}

Table* CreatTable()
{
    char* ret {reinterpret_cast<char*>(malloc(strlen(TABLE_TITLE) + 1))};

    return ret ? strcpy(ret,TABLE_TITLE) : ret;
}

Table* InsertRow(Table *table,RowInfo *info)
{
    char* ret{};

    if (table && info){

        char* t {reinterpret_cast<char*>(table)};

        char* buf {reinterpret_cast<char*>(malloc(strlen(ROW_FORMAT) + sizeof(*info)))};
        
        if (buf){

            sprintf(buf,ROW_FORMAT,info->link,info->name,
                    info->type,info->size,info->time);//把数据插入表格的行

            const int len (strlen(t));//获取表格标题的长度

            ret = reinterpret_cast<char*>(realloc(t,len + strlen(buf) + 1));

            ret = ret ? (strcpy(ret + len ,buf)),ret : t;//把行插入到表格头标题的后面，
                                                        //如有内存分配失败，则返回只是返回标题
        }else{

            ret = t;
        }

        free(buf);
    }

    return ret;
}

char* ToTableString(Table *table)
{
    int const BEGIN_LEN (strlen(TABLE_BEGIN));
    int const END_LEN (strlen(TABLE_END));
    auto t {reinterpret_cast<const char *>(table)};
    char * ret {};

    if ( t && (ret = reinterpret_cast<char*>(malloc(BEGIN_LEN + strlen(t) + END_LEN + 1))) ){
        
        strcpy(ret,TABLE_BEGIN);
        strcpy(ret + BEGIN_LEN,t);
        strcpy(ret + BEGIN_LEN + strlen(t),TABLE_END);
    }

    return ret;
}

void FreeTable(Table *table)
{
    free(table);
}

char* ToErrString(const char* path)
{
    char* ret {};

    if (path && (ret = reinterpret_cast<char*>(malloc(strlen(ERR_FORMAT) + strlen(path) + 1)))){
        sprintf(ret,ERR_FORMAT,path);
    }

    return ret;
}
