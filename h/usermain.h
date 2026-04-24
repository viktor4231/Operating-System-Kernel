#include "../h/riscv.hpp"
#include "../h/thread.hpp"
void userMain();
void userMainWrapper(void*){
    Riscv::uMode = 0;
    userMain();
}
