#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

using namespace std;

void *thread_entry(void * args)
{
    pthread_t id {pthread_self()};

    long n {reinterpret_cast<long >(args)};

    long i {};

    while (i < n)
    {
        std::cout << "id = " << id << "," << "i = " << i << std::endl;

        sleep(1);
        ++i;
    }
    
    return nullptr;
}

int main(int argc, const char** argv) 
{
    pthread_t t1{};
    pthread_t t2{};

    int arg1 {5};
    int arg2 {10};

    std::cout << "sizeof(int):" << sizeof(int) << std::endl;
    std::cout << "sizeof(long):" << sizeof(long) << std::endl;
    std::cout << "sizeof(long long):" << sizeof(long long) << std::endl;
    std::cout << "sizeof(void *):" << sizeof(void *) << std::endl;

    std::cout << "create thread......" << std::endl;

    pthread_create(&t1,nullptr,thread_entry,reinterpret_cast<void*>(arg1));
    pthread_create(&t2,nullptr,thread_entry,reinterpret_cast<void*>(arg2));

    std::cout << "t1 = " << t1 << std::endl;
    std::cout << "t2 = " << t2 << std::endl;

    std::cout << "thread create successed" << std::endl;

    pthread_join(t1,nullptr);
    pthread_join(t2,nullptr);

    return 0;
}

