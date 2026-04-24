//
// Created by os on 7/8/23.
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_SCHEDULER_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_SCHEDULER_HPP

#include "../h/MemoryAllocator.hpp"

class ThreadA;

class Scheduler
{
private:
    struct Elem
    {
        ThreadA *data;
        Elem *next = nullptr;

    };

    static Elem *head;


public:
    static ThreadA *get();

    static void put(ThreadA *ccb);

};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_SCHEDULER_HPP
