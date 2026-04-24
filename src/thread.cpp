//
// Created by os on 7/8/23.
//

#include "../h/thread.hpp"
#include "../h/riscv.hpp"
#include "../h/print.hpp"

ThreadA::Asleep* ThreadA::list = nullptr;

ThreadA *ThreadA::running = nullptr;

uint64 ThreadA::timeSliceCounter = 0;
uint64 ThreadA::change = 0;

ThreadA *ThreadA::createThread(Body body, void* args, uint64* stack_space)
{
    return new ThreadA(body, TIME_SLICE, args, stack_space);
}

void ThreadA::yield()
{
    __asm__ volatile ("ecall");
}

void ThreadA::dispatch()
{
    ThreadA::timeSliceCounter = 0;
    uint volatile sepc = Riscv::r_sepc();
    uint volatile sstatus = Riscv::r_sstatus();
    ThreadA *old = running;
    if (!old->isFinished() && running->isAsleep == 0 && running->isJoined == 0 && running->isBlocked == 0) {
        Scheduler::put(old);
    }
    if(old->isFinished()){
        Joined* prev = nullptr;
        while(old->joined){
            prev = old->joined;
            old->joined->thread->isJoined = 0;
            Scheduler::put(old->joined->thread);
            old->joined = old->joined->next;
            MemoryAllocator::deallocate(prev);
        }
    }
    running = Scheduler::get();

    ThreadA::contextSwitch(&old->context, &running->context);
    Riscv::w_sstatus(sstatus);
    Riscv::w_sepc(sepc);
}

void ThreadA::threadWrapper()
{
    Riscv::popSppSpie();
    running->body(running->args);
    running->setFinished(true);
    ThreadA::yield();
}

void ThreadA::sleep(uint64 t) {
    if(t == 0) return;
  //  Asleep* asl = new Asleep;
    Asleep* asl = (Asleep*)MemoryAllocator::allocate(sizeof(Asleep));
    asl->time = t;
    asl->thread = ThreadA::running;
    asl->next = ThreadA::list;
    ThreadA::list = asl;
    running->isAsleep = 1;
}

void ThreadA::join(ThreadA* t){
    if (ThreadA::running == t || !t) return;
 //   Joined* jnd = new Joined;
    Joined* jnd = (Joined*)MemoryAllocator::allocate(sizeof(Joined));
    jnd->thread = ThreadA::running;
    jnd->next = t->joined;
    t->joined = jnd;
    ThreadA::running->isJoined = 1;
}


