//
// Created by os on 7/8/23.
//

#include "../lib/mem.h"
#include "../h/MemoryAllocator.hpp"

using size_t = decltype(sizeof(0));

void *operator new[](size_t n)
{
  //  return __mem_alloc(n);
    return MemoryAllocator::allocate(n);
}
/*
void *operator new(size_t n)
{
       return __mem_alloc(n);
//    return MemoryAllocator::allocate(n);
}

void operator delete(void *p) noexcept
{
    __mem_free(p);
//    MemoryAllocator::deallocate(p);
}
*/

void operator delete[](void *p) noexcept
{
  //  __mem_free(p);
    MemoryAllocator::deallocate(p);
}
