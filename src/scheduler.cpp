//
// Created by os on 7/8/23.
//

#include "../h/scheduler.hpp"
#include "../lib/mem.h"

Scheduler::Elem* Scheduler::head = nullptr;

ThreadA *Scheduler::get()
{
    if (!head) { return nullptr; }

    Elem *elem = head;
    Elem* prev = nullptr;
    Elem* pprev = nullptr;

    while(elem){
        pprev = prev;
        prev = elem;
        elem = elem->next;
    }
    if(pprev) pprev->next = nullptr;
    ThreadA *ret = prev->data;
    MemoryAllocator::deallocate(prev);
    return ret;
   // return readyThreadQueue.removeFirst();
}

void Scheduler::put(ThreadA *ccb)
{
    Elem *elem = (Elem*)MemoryAllocator::allocate(sizeof(Elem));
    elem->next = Scheduler::head;
    elem->data = ccb;
    head = elem;

  //  readyThreadQueue.addLast(ccb);
}