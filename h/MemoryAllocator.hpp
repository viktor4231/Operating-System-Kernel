//
// Created by os on 7/8/23.
//

#ifndef _MEMORYALLOCATOR_HPP
#define _MEMORYALLOCATOR_HPP

#include "../lib/hw.h"
#include "syscall_cpp.hpp"

class MemoryAllocator{
public:
    static void* allocate(size_t size);
    static void deallocate(void*);
    struct FreeMem{
        FreeMem* next = nullptr;
        size_t size = (((char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR) - 16);
    };
    static int tryToJoin(FreeMem* cur);
    static FreeMem* fMemHead;
    private:
        MemoryAllocator() = default;
        ~MemoryAllocator() = default;
     //   static FreeMem* fMemHead;
    };

#endif

