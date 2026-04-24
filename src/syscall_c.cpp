//
// Created by os on 7/29/23.
//
#include "../h/syscall_c.h"
#include "../h/print.hpp"
#include "../h/thread.hpp"
#include "../h/SemaphoreA.hpp"
#include "../lib/console.h"

void* mem_alloc (size_t size){
    size_t s;
    void* x;
    s = (size + MEM_BLOCK_SIZE - 1)/MEM_BLOCK_SIZE;
    asm volatile("mv a1, %0" : : "r"(s));
    asm volatile("mv a0, %0" : : "r"(0x01));
    asm("ecall");
    asm volatile("mv %0, a0" : "=r"(x));
    return x;
}

int mem_free (void* p){
    uint64 x;
    asm volatile("mv a1, %0" : : "r"(p));
    asm volatile("mv a0, %0" : : "r"(0x02));
    asm("ecall");
    asm volatile("mv %0, a0" : "=r"(x));
    return x;
}

int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg){
    uint64 x;
    void* stack_space = new uint64[DEFAULT_STACK_SIZE];
    asm volatile("mv a7, %0" : : "r"(stack_space));
    asm volatile("mv a3, %0" : : "r"(arg));
    asm volatile("mv a2, %0" : : "r"(start_routine));
    asm volatile("mv a1, %0" : : "r"(handle));
    asm volatile("mv a0, %0" : : "r"(0x11));
    asm("ecall");
    asm volatile("mv %0, a0" : "=r"(x));
    return x;
}

int thread_exit (){
    uint64 x;
    asm volatile("mv a0, %0" : : "r"(0x12));
    asm("ecall");
    asm volatile("mv %0, a0" : "=r"(x));
    if (ThreadA::running == nullptr) return -1;
    thread_dispatch();
    return x;
}

void thread_dispatch (){
    asm volatile("mv a0, %0" : : "r"(0x13));
    asm("ecall");
  /*  for (uint64 j = 0; j < 1; j++)
    {
        for (uint64 k = 0; k < 30000; k++)
        {
            // busy wait
        }
    }*/
}

void thread_join (thread_t handle){
    asm volatile("mv a1, %0" : : "r"(handle));
    asm volatile("mv a0, %0" : : "r"(0x14));
    asm("ecall");
}

int sem_open ( sem_t* handle, unsigned init){
    uint64 x;
    asm volatile("mv a2, %0" : : "r"(init));
    asm volatile("mv a1, %0" : : "r"(handle));
    asm volatile("mv a0, %0" : : "r"(0x21));
    asm("ecall");
    asm volatile("mv %0, a0" : "=r"(x));
    return x;
}

int sem_close (sem_t handle){
    if(!handle) return -1;
    asm volatile("mv a1, %0" : : "r"(handle));
    asm volatile("mv a0, %0" : : "r"(0x22));
    asm("ecall");
    return 0;
}

int sem_wait (sem_t id){
    uint64 x;
    asm volatile("mv a1, %0" : : "r"(id));
    asm volatile("mv a0, %0" : : "r"(0x23));
    asm("ecall");
    asm volatile("mv %0, a0" : "=r"(x));
    for (uint64 j = 0; j < 5000; j++)
    {
        for (uint64 k = 0; k < 30000; k++)
        {
            // busy wait
        }
    }
    if(ThreadA::running->semDeallocated == 1) {
        ThreadA::running->semDeallocated = 0;
        return -2;
    }
    return x;
}

int sem_signal (sem_t id){
    uint64 x;
    asm volatile("mv a1, %0" : : "r"(id));
    asm volatile("mv a0, %0" : : "r"(0x24));
    asm("ecall");
    asm volatile("mv %0, a0" : "=r"(x));
    return x;
}

int time_sleep (time_t time){
  //  int x;
    asm volatile("mv a1, %0" : : "r"(time));
    asm volatile("mv a0, %0" : : "r"(0x31));
    asm("ecall");
    for (uint64 j = 0; j < 10000; j++)
    {
        for (uint64 k = 0; k < 30000; k++)
        {
            // busy wait
        }
    }
 //   ThreadA::dispatch();
  //  asm volatile("mv %0, a0" : "=r"(x));
    return 0;
}

char getc (){
    char x;
    asm volatile("mv a0, %0" : : "r"(0x41));
    asm("ecall");
    asm volatile("mv %0, a0" : "=r"(x));
    return x;
}

void putc (char c){
    asm volatile("mv a1, %0" : : "r"(c));
    asm volatile("mv a0, %0" : : "r"(0x42));
    asm("ecall");
}



