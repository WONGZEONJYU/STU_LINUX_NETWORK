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

static void SortFileEntry(FileEntry* fe)
{
    RowInfo* temp {};

    if (fe && (temp = reinterpret_cast<RowInfo*>(malloc(sizeof(*temp))))){

        for (int i {}; i < fe->length; i++){
            
            int min {i};

            for (int j {i};j < fe->length;++j){

                if (strcmp(fe->data[min].name,fe->data[j].name) > 0){
                    min = j;
                }
            }

            *temp = fe->data[i];
            fe->data[i] = fe->data[min];
            fe->data[min] = *temp;
        }
    }

    free(temp);
}

static int MakeEntryItem(RowInfo* item,dirent* dp,const char* ap,const char* req)
{
    int ret {};
    char *path {GetAbsPath(dp->d_name,ap)};
    struct stat sb{};

    if (path && (ret = (stat(path,&sb) != -1))){

        strcpy(item->link,req);
        (strcmp(req,"/") != 0) ? strcat(item->link,"/") : 0;
        strcat(item->link,dp->d_name);
        
        strcpy(item->name,dp->d_name);
        
        {
            char buf[32]{};

            if (TypeFile == dp->d_type){
                strcpy(item->type,"File");
                if (sb.st_size < 1024){ //bytes
                    sprintf(buf,"%ld",sb.st_size);
                    strcpy(item->size,buf);
                    strcat(item->size," KB");

                }else if((sb.st_size / 1024) < 1024) {   //kbytes
                    sprintf(buf,"%ld",sb.st_size / 1024);
                    strcpy(item->size,buf);
                    strcat(item->size," kB");
                }else{  //Mbytes
                    sprintf(buf,"%ld",sb.st_size / 1024 / 1024);
                    strcpy(item->size,buf);
                    strcat(item->size," MB");
                }
            }else{
                strcpy(item->type,"Folder");
                sprintf(buf,"%d",GetEntryCount(path));
                strcpy(item->size,buf);
                strcat(item->size," Item");
            }
        }

        strcpy(item->time,ctime(&sb.st_mtime));
    }

    free(path);

    return ret;
}

static FileEntry* GetEntry(const char* req, const char* root, const int type)
{
    char* ap {GetAbsPath(req,root)};
    DIR* dirp {};
    FileEntry* ret {};

    if (ap && (dirp = opendir(ap))){
        
        int* pLen{};
        ret = reinterpret_cast<FileEntry*>(malloc(sizeof(*ret)));

        if (ret){
            
            pLen = const_cast<int*>(&ret->length);
            *pLen = 0;
        }

        dirent* dp{};
        int max{};
        constexpr int STEP {5};
        while (pLen && (dp = readdir(dirp))){

            if (*pLen == max){

                max += STEP;
                ret = reinterpret_cast<FileEntry*>(realloc(ret,sizeof(*ret) + sizeof(RowInfo) * max));
                pLen = const_cast<int*>(&ret->length);
            }
            
            if (ret && ( (TypeAll == type) || (type == dp->d_type) )){
                
                if ((!IsDotPath(dp->d_name)) && MakeEntryItem(&ret->data[*pLen],dp,ap,req)){
                    ++(*pLen);
                }
            }
        }

        SortFileEntry(ret);
    }

    free(ap);
    closedir(dirp);

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

    char* ap{MakeHTML("/CMakeFiles", "/home/wong/STU_LINUX_NETWORK/23.file_server/build")};

    std::cout << ap << std::endl;

    free(ap);

    return ret;
}
