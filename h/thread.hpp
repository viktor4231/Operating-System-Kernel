//
// Created by os on 7/8/23.
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"
#include "../h/print.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/riscv.hpp"

// ThreadA Control Block
class ThreadA
{
public:
    ~ThreadA() {
        delete[]stack; //delete joined; delete list;
    //    MemoryAllocator::deallocate(stack);
        MemoryAllocator::deallocate(joined);
        MemoryAllocator::deallocate(list);
    }

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    uint64 getTimeSlice() const { return timeSlice; }

    using Body = void (*)(void*);

    static ThreadA *createThread(Body body, void* args, uint64* stack_space);

    static void yield();

    static ThreadA *running;

    struct Asleep{
        ThreadA* thread;
        uint64 time;
        Asleep* next = nullptr;
    };

    struct Joined{
        ThreadA* thread;
        Joined* next;
    };


    static void sleep(uint64);
    static Asleep* list;
 //   uint64 timeSlice;
    static uint64 change;
    uint64 isDispatched = 0;
    uint64 isAsleep = 0;
    uint64 isJoined = 0;
    uint64 isBlocked = 0;
    uint64 semDeallocated = 0;
    Joined* joined = nullptr;

    static void join(ThreadA*);

    void *operator new(size_t n)
    {
        return MemoryAllocator::allocate(n);
    }

    void operator delete(void *p)
    {
        MemoryAllocator::deallocate(p);
    }

private:
    ThreadA(Body body, uint64 timeSlice, void* args, uint64* stack_space) :
            body(body),
          //  stack(body != nullptr ? stack_space : nullptr),
            stack(stack_space),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64) &stack[STACK_SIZE] : 0
                    }),
            timeSlice(timeSlice),
            args(args),
            finished(false)
    {
        if (body != nullptr) { Scheduler::put(this); }
       // else MemoryAllocator::deallocate(stack_space);
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    uint64 *stack;
  //  uint64 start = 0;
    Context context;
public:
    uint64 timeSlice;
private:
    void* args;
    bool finished;

    friend class Riscv;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);
public:
    static void dispatch();
private:
    static uint64 timeSliceCounter;

    static uint64 constexpr STACK_SIZE = DEFAULT_STACK_SIZE;
    static uint64 constexpr TIME_SLICE = DEFAULT_TIME_SLICE;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
