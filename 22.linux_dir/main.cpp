#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

void func1()
{
    DIR * dirp {opendir(".")};

    if (dirp){
        
        dirent64 * dp{};

        while ((dp = readdir64(dirp))){

            struct stat64 sb64 {};
            if (stat64(dp->d_name,&sb64) != -1){

                std::cout << "name:"<< dp->d_name << " type:" << std::hex << static_cast<int>(dp->d_type)
                << " len:" << sb64.st_size << " mtime:" << ctime(&sb64.st_mtime) << std::endl;
            }
        }

        // dirent * dp {};

        // while ((dp = readdir(dirp))){
            
        //     struct stat sb {};
        //     if (stat(dp->d_name,&sb) != -1){
        //         std::cout << "name:"<< dp->d_name << " type:" << std::hex << static_cast<int>(dp->d_type)
        //         << " len:" << sb.st_size << " mtime:" << ctime(&sb.st_mtime) << std::endl;
        //     }
        // }

        closedir(dirp);    
    }
}

void file_copy(const char * dst,const char * src)
{
    int dfd {open(dst,O_WRONLY|O_CREAT,0600)};
    int sfd {open(src,O_RDONLY)};

    if ((-1 != dfd) && (-1 != sfd)){
        
        char buf[512]{};
        int len{};

        while ((len = read(sfd,buf,sizeof(buf))) > 0){
            write(dfd,buf,len);
        }

        close(dfd);
        close(sfd);
    }
}


int main(int argc, const char** argv) 
{

    //func1();
    //file_copy("new.out","a.out");
    cout << "argc : " << argc << endl;
    cout << "argv[0] :" << argv[0] << endl;
    cout << "argv[1] :" << argv[1] << endl;

    return 0;
}

