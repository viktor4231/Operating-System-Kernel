//
// Created by os on 7/8/23.
//
#include "../h/syscall_cpp.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../lib/mem.h"
#include "../h/thread.hpp"
#include "../lib/console.h"
#include "../h/SemaphoreA.hpp"

void *operator new(size_t n)
{
    return mem_alloc(n);
}

void operator delete(void *p)
{
    mem_free(p);
}

Thread::Thread(void (*body)(void *), void *arg) {
    this->body = body;
    this->arg = arg;
    this->myHandle = nullptr;
}

int Thread::start() {
    if(body){
        return thread_create(&myHandle, body, arg);
    }
    else return -1;
}

void Thread::dispatch() {
    thread_dispatch();
}

void Thread::join() {
    thread_join(myHandle);
}

int Thread::sleep(time_t t) {
    return time_sleep(t);
}

Thread::Thread() {
    this->body = runWrapper;
    this->arg = this;
    this->myHandle = nullptr;
}

void Thread::runWrapper(void* thread){
    Thread* t = (Thread*)thread;
    t->run();
}

Thread::~Thread() {
    delete myHandle;
};


Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    delete myHandle;
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}
