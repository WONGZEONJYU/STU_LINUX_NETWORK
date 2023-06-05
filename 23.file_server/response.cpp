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

/*通过请求路径和根路径拼接成绝对路径*/
static char* GetAbsPath(const char* relative,const char* root)
{
    const int reLen (strlen(relative));
    const int rootLen (strlen(root));

    char *ret {reinterpret_cast<char*>(malloc(reLen + rootLen + 2))};

    if (ret){

        strcpy(ret,root);

        if ( ('/' == relative[0]) && ('/' == ret[rootLen - 1]) ){
            ret[rootLen - 1] = 0;
            /*
            如果请求路径的头部存在分隔符'/'和根目录路径尾部也存在一个分隔符'/',则删除根目录尾部分隔符"/",再进行拼接
            such as: root = "/root/"  relative = "/file" ===> /root/ + /file ===>
                /root + /file ==> /root/file
            */
        }

        if( ('/' != relative[0]) && ('/' != ret[rootLen - 1]) ){
            /*
                如果请求路径头部和根目录路径尾部都没有'/'分隔符,则在根目录后面加上"/"，再进行拼接
                such as: root = "/root" relative = "folder" ===> /root + folder ===> /root + /folder
                ===>    /root/folder
            */
            strcat(ret,"/");
        }

        strcat(ret,relative);
    }

    return ret;
}

/*判断路径是否为"." 或者 ".." ,yes return 1 , no return 0 , error return -1 */
static int IsDotPath(const char* path)
{
    int ret {-1};

    if (path){
        ret = ((0 == strcmp(path,".")) || (0 == strcmp(path,"..")));
    }

    return ret;
}

/*获取文件和文件夹的个数，除了"." , ".." 以外的个数*/
static int GetEntryCount(const char* path)
{
    int ret {-1};

    DIR * dirp {opendir(path)};

    if (dirp){
        ret = 0;
        while (dirent *dp {readdir(dirp)}){
            if (!IsDotPath(dp->d_name)){
                ++ret;
            }
        }
    }

    closedir(dirp);

    return ret;
}

/*对文件和文件夹按名字进行从小到大的排序,*/
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

/*根据dp所指向的 (文件 和 文件夹) 生成RowInfo数据结构，成功返回true，失败返回false*/
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
                if (sb.st_size < 1024){                     //bytes
                    sprintf(buf,"%ld",sb.st_size);
                    strcpy(item->size,buf);
                    strcat(item->size," KB");

                }else if((sb.st_size / 1024) < 1024) {      //kbytes

                    sprintf(buf,"%ld",sb.st_size / 1024);
                    strcpy(item->size,buf);
                    strcat(item->size," kB");
                }else{                                      //Mbytes

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

/* 获取root/req 路径下 (文件 和 文件夹) 所对应的RowInfo数组，返回值需释放 (free) */
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

/*创建root/req 路径下 (文件 和 文件夹) 所构成的 HTML 页面，返回值需释放 (free）*/
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

static int Response(TcpClient* client,const char* html)
{
    const char* HTTP_FORMAT {
        "HTTP:/1.1 200 OK\r\n"
        "Server:Test Http Server\r\n"
        "Content-Length:%d\r\n"
        "Content-Type:text/html\r\n"
        "Connection:close\r\n\r\n"
        "%s"
    };

    int ret {};

    if (html){
        
        const size_t html_size {strlen(html)};

        char* resp {reinterpret_cast<char*>(malloc(strlen(HTTP_FORMAT) + html_size + 16))};

        if (resp){

            sprintf(resp,HTTP_FORMAT,html_size,html);

            ret = (TcpClient_SendRaw(client,resp,strlen(resp)) > 0);
        }

        free(resp);
    }

    return ret;
}

static int DirReqHandler(TcpClient* client,const char* req,const char* root)
{
    char* html { MakeHTML(req,root) };
    int ret { Response(client,html) };
    free(html);
    return ret;
}

static int BadReqHandler(TcpClient* client,const char* req,const char* root)
{
    char* html { ToErrString(req)};
    int ret { Response(client,html) };
    free(html);
    return ret;
}

static int FileReqHandler(TcpClient* client,const char* req,const char* root)
{
    const char* HTTP_FORMAT {
        "HTTP/1.1 200 OK\r\n"
        "Server:Test Http Server\r\n"
        "Content-Length:%d\r\n"
        "Content-Type:application/*\r\n"
        "Connection:close\r\n\r\n"
    };

    constexpr int BUF_SIZE {1024};

    int ret {};
    char* ap {GetAbsPath(req,root)};
    const int fd {open(ap,O_RDONLY)};
    char* head {reinterpret_cast<char*>(malloc(strlen(HTTP_FORMAT) + 32))};
    char* buf {reinterpret_cast<char*>(malloc(BUF_SIZE))};

    if (ap && head && buf && (-1 != fd)){

        int max (lseek(fd,0,SEEK_END));

        sprintf(head,HTTP_FORMAT,max);

        int len {TcpClient_SendRaw(client,head,strlen(head))};

        lseek(fd,0,SEEK_SET);

        while ( (len > 0) && ((len = read(fd,buf,BUF_SIZE)) > 0) ){
            ret += TcpClient_SendRaw(client,buf,len);
        }
        ret = (ret == max);
    }

    free(buf);
    free(head);
    free(ap);
    close(fd);

    return ret;
}

int RequestHandler(TcpClient* client,const char* req,const char* root)
{
    int ret {};

    if (client && req && root){

        char* ap {GetAbsPath(req,root)};

        if ( ap && (0 == access(ap,F_OK)) ){
            if (GetEntryCount(ap) < 0){
                ret = FileReqHandler(client,req,root);
            }else {
                ret = DirReqHandler(client,req,root);
            }
        }else{
            ret = BadReqHandler(client,req,root);
        }
        free(ap);
    }

    return ret;
}
