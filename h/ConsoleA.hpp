//
// Created by os on 7/8/23.
//

#ifndef PROJECT_BASE_CONSOLEA_HPP
#define PROJECT_BASE_CONSOLEA_HPP

#include "../lib/hw.h"
#include "../h/print.hpp"
#include "../lib/console.h"

class ConsoleA{
public:
    static char getchar();
    static void putchar(char);
    static char inbuffer[100];
    static char outbuffer[100];
    static uint16 inbuffLength;
    static uint16 outbuffLength;
};


#endif //PROJECT_BASE_CONSOLEA_HPP
