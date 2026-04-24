//
// Created by os on 7/8/23.
//

#include "../h/thread.hpp"
#include "../h/workers.hpp"
#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
#include "../h/MemoryAllocator.hpp"
#include "../h/syscall_cpp.hpp"
#include "../lib/console.h"
#include "../h/ConsoleA.hpp"
#include "../h/usermain.h"



int main() {
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap | 0x01);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);



    ThreadA *kernelNit;
    ThreadA *uMain;
    thread_create(&kernelNit, nullptr, nullptr);
    ThreadA::running = kernelNit;

    thread_create(&uMain, userMainWrapper, nullptr);


    while(!uMain->isFinished()){
        ThreadA::dispatch();
      //  thread_dispatch();
    }



    delete uMain;
    delete kernelNit;
    return 0;
}
