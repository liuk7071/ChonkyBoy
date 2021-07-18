#pragma once
#include <array>
#include <string>
#include <sstream>
#include "utils.hpp"
#include "BitField.hpp"
#include "memory.hpp"

class cpu {
public:
    cpu(memory* memptr);
    memory* Memory;
    // Registers
    union {     // AF
        u16 af;
        BitField <8, 8, u16> a;
        u8 f;
        BitField <7, 1, u16> Z;
        BitField <6, 1, u16> N;
        BitField <5, 1, u16> HC;
        BitField <4, 1, u16> C;
    };
    union {     // BC
        u16 bc;
        BitField <0, 8, u16> c;
        BitField <8, 8, u16> b;
    };
    union {     // DE
        u16 de;
        BitField <0, 8, u16> e;
        BitField <8, 8, u16> d;
    };
    union {     // HL
        u16 hl;
        BitField <0, 8, u16> l;
        BitField <8, 8, u16> h;
    };
    
    u16 pc;
    u16 sp;

    void execute(u16 address);
};