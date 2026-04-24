//
// Created by os on 8/11/23.
//

#include "../h/ConsoleA.hpp"

char ConsoleA::inbuffer[100];
char ConsoleA::outbuffer[100];
uint16 ConsoleA::inbuffLength = 0;
uint16 ConsoleA::outbuffLength = 0;

char ConsoleA::getchar() {
    if(inbuffLength > 0){
        return inbuffer[inbuffLength--];
    }
    return 0;
}

void ConsoleA::putchar(char c) {
  //  __putc(c);
    if(outbuffLength < 100){
        outbuffer[outbuffLength++] = c;
    }
}



