//
// Created by os on 7/8/23.
//
#include "../h/MemoryAllocator.hpp"
#include "../h/print.hpp"
#include "../lib/console.h"

MemoryAllocator::FreeMem* MemoryAllocator::fMemHead = nullptr;

void *MemoryAllocator::allocate(size_t size) {
    if(!fMemHead){
    fMemHead = (FreeMem*)(char*)HEAP_START_ADDR;
    fMemHead->size = ((char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR) - 16;
    }
    FreeMem* blk = fMemHead, *prev = nullptr;
    for(; blk != nullptr; prev = blk, blk = blk->next) { if (blk->size >= size) break;}
    if (blk == nullptr) return nullptr;
    size_t remainingSize = blk->size - size;
    if (remainingSize >= 16){
        // A fragment remains
        blk->size = size;
        size_t offset = 16 + size;
        FreeMem* newBlk = (FreeMem*)((char*)blk + offset);
        if(prev) prev->next = newBlk;
        else fMemHead = newBlk;
        newBlk->next = blk->next;
        newBlk->size = remainingSize - 16;
    }
    else{
        // No remaining fragment, allocate the entire block
        if(prev) prev->next = blk->next;
        else fMemHead = blk->next;
    }
    blk->next = nullptr;
    return(char*)blk + 16;
}

void MemoryAllocator::deallocate(void* addr){
    if(addr == nullptr) return;
    FreeMem* cur = 0;
    if(!fMemHead || addr<(char*)fMemHead) cur = 0; // Insert as first
    else for(cur = fMemHead; cur->next != 0 && addr > (char*)(cur->next); cur = cur->next);
    //Insert the new segment after cur
    FreeMem* newSeg = (FreeMem*)((char*)addr - 16);
    if(cur) newSeg->next = cur->next;
    else newSeg->next = fMemHead;
    if(cur) cur->next = newSeg;
    else fMemHead = newSeg;
    tryToJoin(newSeg);
    tryToJoin(cur);
}

int MemoryAllocator::tryToJoin(MemoryAllocator::FreeMem *cur) {
    if(!cur) return 0;
    if(cur->next && (char*)cur+cur->size + 16 == (char*)(cur->next)){
        // Remove the cur->next segment;
        cur->size += cur->next->size + 16;
        cur->next = cur->next->next;
        return 1;
    }
    else return 0;
}
