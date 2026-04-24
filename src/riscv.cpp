//
// Created by os on 7/8/23.
//

#include "../h/riscv.hpp"
#include "../h/thread.hpp"
#include "../h/SemaphoreA.hpp"
#include "../lib/console.h"
#include "../h/print.hpp"
#include "../h/syscall_c.h"
#include "../h/ConsoleA.hpp"

uint64 Riscv::uMode = 1;

void Riscv::popSppSpie()
{
    if(uMode) ms_sstatus(SSTATUS_SPP);
    else mc_sstatus(SSTATUS_SPP);
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSupervisorTrapE() {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
    uint64 scause = r_scause();
 //   if (scause == 0x0000000000000002UL) {printStringA("\nGreska: ilegalna instrukcija\n");
 //       mc_sip(SIP_SSIP);
 //       thread_exit();
 //   };
 //   if (scause == 0x0000000000000005UL) printStringA("\nGreska: nedozvoljena adresa citanja\n");
 //   if (scause == 0x0000000000000007UL) printStringA("\nGreska: nedozvoljena adresa upisa\n");
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) {
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();
        uint64 operacija;
        asm volatile("mv %0, a0" : "=r"(operacija)); // citanje iz a0
        switch (operacija) {
            case 0x01:
                uint64 size;
                void *adres;
                asm volatile("mv %0, a1" : "=r"(size));
                adres = MemoryAllocator::allocate(size * MEM_BLOCK_SIZE);
                asm volatile("sd %0, 8*10(fp)" : : "r"(adres));
                break;
            case 0x02:
                void *adr;
                asm volatile("mv %0, a1" : "=r"(adr));
                MemoryAllocator::deallocate(adr);
                asm volatile("sd %0, 8*10(fp)" : : "r"(0));
                break;
            case 0x11: //thread create
                ThreadA **handle;
                void (*start_routine)(void *);
                void *arg;
                uint64 *stack_space;
                asm volatile("mv %0, a7" : "=r"(stack_space));
                asm volatile("mv %0, a3" : "=r"(arg));
                asm volatile("mv %0, a2" : "=r"(start_routine));
                asm volatile("mv %0, a1" : "=r"(handle));
                printStringA("");
                *handle = ThreadA::createThread(start_routine, arg, stack_space);
                if (*handle) asm volatile("sd %0, 8*10(fp)" : : "r"(0));
                else asm volatile("sd %0, 8*10(fp)" : : "r"(-1));
                break;
            case 0x12: // thread exit
                asm volatile("sd %0, 8*10(fp)" : : "r"(0));
                ThreadA::running->setFinished(true);
                ThreadA::timeSliceCounter = 0;
                break;
            case 0x13: //thread dispatch
                ThreadA::dispatch();
                //   ThreadA::running->isDispatched = 1;
                break;
            case 0x14:
                ThreadA *handleJ;
                asm volatile("mv %0, a1" : "=r"(handleJ));
                ThreadA::join(handleJ);
                break;
            case 0x21: // Open semaphore
                SemaphoreA **handl;
                uint64 init;
                asm volatile("mv %0, a2" : "=r"(init));
                asm volatile("mv %0, a1" : "=r"(handl));
                *handl = SemaphoreA::createSemaphore(init);
                if (*handl) asm volatile("sd %0, 8*10(fp)" : : "r"(0));
                else asm volatile("sd %0, 8*10(fp)" : : "r"(-1));
                break;
            case 0x22: // Close Semaphore
                SemaphoreA *hdl;
                asm volatile("mv %0, a1" : "=r"(hdl));
                SemaphoreA::closeSemaphore(hdl);
                break;
            case 0x23: // Semaphore wait
                SemaphoreA *sem;
                uint64 cd;
                asm volatile("mv %0, a1" : "=r"(sem));
                cd = sem->wait();
                asm volatile("sd %0, 8*10(fp)" : : "r"(cd));
                break;
            case 0x24: // Semaphore signal
                SemaphoreA *sem1;
                uint64 cod;
                asm volatile("mv %0, a1" : "=r"(sem1));
                cod = sem1->signal();
                asm volatile("sd %0, 8*10(fp)" : : "r"(cod));
                break;
            case 0x31:
                uint64 time;
                asm volatile("mv %0, a1" : "=r"(time));
                ThreadA::sleep(time);
                break;
            case 0x41:
                char c;
              //  c = ConsoleA::getchar();
                c = __getc();
                asm volatile("sd %0, 8*10(fp)" : : "r"(c));
                break;
            case 0x42:
                char cr;
                asm volatile("mv %0, a1" : "=r"(cr));
                __putc(cr);
             //   ConsoleA::putchar(cr);
                break;
            default:
                ThreadA::timeSliceCounter = 0;
                ThreadA::dispatch();
                break;
        }
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
}
void Riscv::handleSupervisorTrapT() {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
        ThreadA::timeSliceCounter++;
        ThreadA::Asleep* a = ThreadA::list;
        ThreadA::Asleep* prev = nullptr;
        while(a){
            a->time -= 1;
     //       printStringA("\n");
     //       printInteger(a->time);
     //       printStringA("\n");
            if(a->time == 0){
                a->thread->isAsleep = 0;
                Scheduler::put(a->thread);
                if(a == ThreadA::list) ThreadA::list = a->next;
                else prev->next = a->next;
            }
            prev = a;
            a = a->next;
        }
    mc_sip(SIP_SSIP);
        if (ThreadA::timeSliceCounter >= ThreadA::running->getTimeSlice() || ThreadA::running->isAsleep == 1 || ThreadA::running->isJoined == 1 || ThreadA::running->isBlocked == 1 || ThreadA::running->isDispatched == 1){
            uint64 volatile sepc = r_sepc();
            uint64 volatile sstatus = r_sstatus();
            ThreadA::running->isDispatched = 0;
            ThreadA::timeSliceCounter = 0;
            ThreadA::dispatch();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }
}
void Riscv::handleSupervisorTrapU() {
    {
        console_handler();
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
     //   while(*(char*)CONSOLE_STATUS & (char)CONSOLE_RX_STATUS_BIT && ConsoleA::inbuffLength < 100) {
     //       ConsoleA::inbuffer[ConsoleA::inbuffLength++] = *(uint64*)CONSOLE_RX_DATA;
     //   }
     //   while(*(char*)CONSOLE_STATUS & (char)CONSOLE_TX_STATUS_BIT && ConsoleA::inbuffLength > 0){
    //        *(char*)CONSOLE_TX_DATA = ConsoleA::inbuffer[--ConsoleA::inbuffLength];
  //        }
     //   plic_complete(10);

    }
}
   // else
   // {
        // unexpected trap cause
   // }
//}