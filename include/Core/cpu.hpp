#pragma once
#include <array>
#include <string>
#include <sstream>
#include <cstdarg>
#include "utils.hpp"
#include "BitField.hpp"
#include "memory.hpp"

class cpu {
public:
    cpu(memory* memptr);
    memory* Memory;
    bool debug_cpu = true;
    void debug(const char* fmt, ...) {
        if (debug_cpu) {
		std::va_list args;
		va_start(args, fmt);
		std::vprintf(fmt, args);
		va_end(args);
	    }
    }

    // Registers
    union {     // AF
        u16 af;
        BitField <8, 8, u16> a;
        BitField <7, 1, u16> Z;
        BitField <6, 1, u16> N;
        BitField <5, 1, u16> HC;
        BitField <4, 1, u16> C;
        BitField <0, 8, u16> f;
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
    union {     // hl
        u16 hl;
        BitField <0, 8, u16> l;
        BitField <8, 8, u16> h;
    };
    
    u16 pc;
    u16 sp;

    template<int group>
    u16 GetR16(int number) {
        if (group == 1) {
            switch (number) {
            case 0:	// BC
                return bc;
            case 1:	// DE
                return de;
            case 2: // HL
                return hl;
            case 3: // SP
                return sp;
            }
        }
        else if (group == 2) {
            switch (number) {
            case 0:	// BC
                return bc;
            case 1:	// DE
                return de;
            case 2: // HL+
                return hl++;
            case 3: // HL-
                return hl--;
            }
        }
        else {
            Helpers::panic("Bad R16 group\n");
        }
    }
	bool CheckForCondition(uint8_t opcode) {
        if (opcode & 1) // There is no condition
            return true;

        switch ((opcode >> 3) & 3) {
        case 0: {	// NZ
            return !Z;
        }
        case 1: {	// Z
            return Z;
        }
        case 2: {	// NC
            return !C;
        }
        case 3: {	// C
            return C;
        }
        }
    }
    u8 Fetch8();
    u16 Fetch16();

    void execute(u8 opcode);
    int frame_cycles = 0;
};