//
// Created by os on 7/8/23.
//
#ifndef PROJECT_BASE_SEMAPHOREA_HPP
#define PROJECT_BASE_SEMAPHOREA_HPP

#include "../lib/hw.h"
#include "../h/thread.hpp"


class SemaphoreA{
private:
    struct Blocked{
        ThreadA* thread;
        Blocked* next;
    };
public:
    ~SemaphoreA(){
        closeSemaphore(this);
        MemoryAllocator::deallocate(blocked);
    }

    SemaphoreA(uint16 init = 1) : val(init){}
    int wait();
    int signal();

    static SemaphoreA* createSemaphore(uint16 init){
        return new SemaphoreA(init);
    }

    static void closeSemaphore(sem_t);

    void *operator new(size_t n)
    {
        return MemoryAllocator::allocate(n);
    }

    void operator delete(void *p)
    {
        MemoryAllocator::deallocate(p);
    }

private:
    int val;
    Blocked* blocked = nullptr;
};


#endif //PROJECT_BASE_SEMAPHOREA_HPP
