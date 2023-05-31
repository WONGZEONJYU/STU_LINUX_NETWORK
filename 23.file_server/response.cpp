#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include "page.h"
#include "response.h"

enum{ TypeAll = 0x00,TypeDir = 0x04,TypeFile = 0x08 };
struct FileEntry
{
    const int length;
    RowInfo data[];
};

static const char* HTTP_FORMAT {
    "HTTP:/1.1 200 OK\r\n"
    "Server:Test Http Server\r\n"
    "Content-Length:%d\r\n"
    "Content-Type:text/html\r\n"
    "Connection:close\r\n\r\n"
};

static char* GetAbsPath(const char* relative,const char* root)
{
    int reLen (strlen(relative));
    int rootLen (strlen(root));

    char *ret {reinterpret_cast<char*>(malloc(reLen + rootLen + 2))};

    if (ret){

        strcpy(ret,root);

        if ( ('/' == relative[0]) && ('/' == ret[rootLen - 1]) ){
            ret[rootLen - 1] = 0;
            /*
            如果请求路径的头部存在分隔符'/'和根目录路径最后也存在一个分隔符'/'
            such as: root = "/root/"  relative = "/file" ===> /root/ + /file ===>
                /root + /file ==> /root/file
            */
        }

        if( ('/' != relative[0]) && ('/' != ret[rootLen - 1]) ){
            //如果请求路径和根目录路径都没有'/'分隔符
            strcat(ret,"/");
        }

        strcat(ret,relative);
    }

    return ret;
}

static int IsDotPath(const char* path)
{
    int ret {-1};

    if (path){
        ret = ((0 == strcmp(path,".")) || (0 == strcmp(path,"..")));
    }

    return ret;
}

static int GetEntryCount(const char* path)
{
    int ret {-1};

    DIR * dirp {opendir(path)};

    if (dirp){
        
        dirent * dp {};
        ret = 0;
        while (dp = readdir(dirp)){

            if (!IsDotPath(dp->d_name)){
                ++ret;
            }
        }
    }

    closedir(dirp);

    return ret;
}

static FileEntry* GetEntry(const char* req, const char* root, int type)
{
    FileEntry * ret {};




    return ret;
}

static char* MakeHTML(const char* req, const char* root)
{
    char* ret{};

    Table* table {CreatTable()};

    if (table){

        RowInfo* back {};

        if ( (strcmp(req,"/") != 0) && (back = reinterpret_cast<RowInfo*>(calloc(1,sizeof(*back)))) ){
            
            int i (strlen(req) - 1);

            strcpy(back->link,req);

            while ('/' != back->link[i]) {
                --i;
            }

            i ? (back->link[i] = 0) : (back->link[i + 1] = 0);
            /*
                such as : req = "/abc/de" ===> parent folder "/abc"
                such as : req = "/abc" ===> parent folder = "/"
            */
           strcpy(back->name,"./..");
           strcpy(back->type,"Back..");
           table = InsertRow(table,back);
        }

        free(back);

        FileEntry* fe {GetEntry(req,root, TypeDir)};
  
        for (int i {}; (fe) && (i < fe->length); i++){

            table = InsertRow(table,&fe->data[i]);
        }

        free(fe);

        fe = GetEntry(req,root, TypeFile);

        for (int i {}; (fe) && (i < fe->length); i++){

            table = InsertRow(table,&fe->data[i]);
        }

        free(fe);

        char* ts{ToTableString(table)};

        ret = ts ? ToPageString(req,ts) : nullptr;

        free(ts);
    }

    FreeTable(table);

    return ret;
}

int RequestHandler(TcpClient* client,const char* req,const char* root)
{
    int ret {};

    // char* ap { GetAbsPath("cd/e","/a/b") };

    // std::cout <<"ap = " << ap << std::endl;
    
    // std::cout <<". = " << IsDotPath(".") << std::endl;
    // std::cout <<".. = " << IsDotPath("..") << std::endl;
    // std::cout <<"/a/b = " << IsDotPath("/a/b") << std::endl;

    // std::cout << GetEntryCount("/home/wong/STU_LINUX_NETWORK/23.file_server/build") << std::endl;

    // free(ap);

    char* ap{MakeHTML("/home/wong", "/home/wong/STU_LINUX_NETWORK/23.file_server/build")};

    std::cout << "ap = " << ap << std::endl;

    free(ap);

    return ret;
}
