//
// Created by os on 8/7/23.
//

#include "../h/SemaphoreA.hpp"

int SemaphoreA::wait() {
    if(--val < 0){
        Blocked* blc = (Blocked*)MemoryAllocator::allocate(sizeof(Blocked));
        blc->thread = ThreadA::running;
        blc->next = this->blocked;
        this->blocked = blc;
        ThreadA::running->isBlocked = 1;
        return 0;
    }
    return -1;
}

int SemaphoreA::signal() {
    if(++val <= 0){
        Blocked* prev = this->blocked;
        this->blocked->thread->isBlocked = 0;
        Scheduler::put(this->blocked->thread);
        this->blocked = this->blocked->next;
        MemoryAllocator::deallocate(prev);
        return 0;
    }
    return -1;
}

void SemaphoreA::closeSemaphore(sem_t s) {
    Blocked* prev = nullptr;
    while(s->blocked){
        prev = s->blocked;
        s->blocked->thread->isBlocked = 0;
        s->blocked->thread->semDeallocated = 1;
        Scheduler::put(s->blocked->thread);
        s->blocked = s->blocked->next;
        MemoryAllocator::deallocate(prev);
    }
}
