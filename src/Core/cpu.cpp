#include "cpu.hpp"

cpu::cpu(memory* memptr) {
    Memory = memptr;

    // Initialize registers if skipping the bootrom
    if(skip_bootrom) {
        a = 0x01;
        f = 0xb0;
        b = 0x0;
        c = 0x13;
        d = 0x0;
        e = 0xd8;
        h = 0x01;
        l = 0x4d;
        sp = 0xfffe;
        pc = 0x100;

        Memory->bootrom_enabled = false;
        Memory->PPU->mode = ppu::VBlank;
        Memory->PPU->ly = 0x91;
        Memory->PPU->lcdc = 0x91;
        Memory->PPU->bgp = 0xfc;
    }
    
}

u8 cpu::Fetch8() {
    frame_cycles += 4;
    return Memory->Read(pc++);
}

u16 cpu::Fetch16() {
    u8 byte1 = Fetch8();
    u8 byte2 = Fetch8();
    return byte1 | (byte2 << 8);
}

void cpu::execute(u8 opcode) {
    switch(opcode) {
    case 0x00: debug("0x%04x | 0x%02x | NOP\n", pc-1, opcode); break;   // NOP
    case 0x01: case 0x11: case 0x21: case 0x31: {   // LD r16, u16
        debug("0x%04x | 0x%02x | LD r16, u16\n", pc-1, opcode);
        u16 value = Fetch16();
        SetR16<1>((opcode >> 4) & 3, value);
        break;
    }
    case 0x02: case 0x12: case 0x22: case 0x32: {   // LD (R16), A
        debug("0x%04x | 0x%02x | LD (R16), A\n", pc-1, opcode);
        u16 addr = GetR16<2>((opcode >> 4) & 3);
        Memory->Write(addr, a.Value());
        break;
    }
    case 0x03: case 0x13: case 0x23: case 0x33: {   // INC r16
        debug("0x%04x | 0x%02x | INC r16\n", pc-1, opcode);
        u16 r16 = GetR16<1>((opcode >> 4) & 3);
        r16++;
        SetR16<1>((opcode >> 4) & 3, r16);
        break;
    }
    case 0x04: case 0x14: case 0x24 :case 0x34: case 0x0c: case 0x1c: case 0x2c: case 0x3c: {   // INC r8
        debug("0x%04x | 0x%02x | INC r8\n", pc-1, opcode);
        u8 r8 = GetR8((opcode >> 3) & 7);
        u8 res = r8 + 1;
        Z = (res == 0);
        N = false;
        HC = ((r8 & 0xf) == 0xf);
        SetR8((opcode >> 3) & 7, res);
        break;
    }
    case 0x05: case 0x15: case 0x25: case 0x35: case 0x0d: case 0x1d: case 0x2d: case 0x3d: {   // DEC r8
        debug("0x%04x | 0x%02x | DEC r8\n", pc-1, opcode);
        u8 r8 = GetR8((opcode >> 3) & 7);
        u8 res = r8 - 1;
        Z = (res == 0);
        N = true;
        HC = ((r8 & 0xf) == 0);
        SetR8((opcode >> 3) & 7, res);
        break;
    }
    case 0x06: case 0x16: case 0x26: case 0x36: case 0x0e: case 0x1e: case 0x2e: case 0x3e: {   // LD r8, u8
        debug("0x%04x | 0x%02x | LD r8, u8\n", pc-1, opcode);
        u8 value = Fetch8();
        SetR8((opcode >> 3) & 7, value);
        break;
    }
    case 0x07: {    // RLCA
        debug("0x%04x | 0x%02x | RLCA\n", pc-1, opcode);
        u8 temp = a;
        a = (a << 1) | ((temp >> 7) & 1);
        Z = false;
        N = false;
        HC = false;
        C = (temp >> 7) & 1; 
        break;
    }
    case 0x08:{ // LD (u16), sp
        debug("0x%04x | 0x%02x | LD (u16), sp\n", pc-1, opcode);
        u16 addr = Fetch16();
        Memory->Write16(addr, sp);
        break;
    }
    case 0x09: case 0x19: case 0x29: case 0x39: {   // ADD HL, r16
        debug("0x%04x | 0x%02x | ADD HL, r16\n", pc-1, opcode);
        u16 r16 = GetR16<1>((opcode >> 4) & 3);
        u32 res = u32(hl + r16);
        N = false;
        HC = (hl & 0xfff) + (r16 & 0xfff) > 0xfff;
        C = (res >> 16) & 1;
        hl += r16;
        break;
    }
    case 0x0a: case 0x1a: case 0x2a: case 0x3a: {   // LD A, (r16)
        debug("0x%04x | 0x%02x | LD A, (r16)\n", pc-1, opcode);
        a = Memory->Read(GetR16<2>((opcode >> 4) & 3));
        break;
    }
    case 0x0b: case 0x1b: case 0x2b: case 0x3b: {   // DEC r16
        debug("0x%04x | 0x%02x | DEC r16\n", pc-1, opcode);
        u16 r16 = GetR16<1>((opcode >> 4) & 3);
        r16--;
        SetR16<1>((opcode >> 4) & 3, r16);
        break;
    }
    case 0x17: {    // RLA
        debug("0x%04x | 0x%02x | RLA\n", pc-1, opcode);
        u8 temp = a.Value();
        u8 carry = C.Value();
        a = (a.Value() << 1) | carry;
        Z = false;
        N = false;
        HC = false;
        C = (temp & 0x80) >> 7;
        break;
    }
    case 0x18: {    // JR s8
        debug("0x%04x | 0x%02x | JR s8\n", pc-1, opcode);
        pc += s8(Fetch8());
        break;
    }
    case 0x1f: {    // RRA
        debug("0x%04x | 0x%02x | RRA\n", pc-1, opcode);
        u8 temp = a.Value();
        u8 carry = C.Value();
        a = (a.Value() >> 1) | (carry << 7);
        Z = false;
        N = false;
        HC = false;
        C = temp & 1;
        break;
    }
    case 0x20: case 0x30: case 0x28: case 0x38: {   // JR Condition s8
        debug("0x%04x | 0x%02x | JR Cond s8\n", pc-1, opcode);
        s8 offs = s8(Fetch8());
        if(CheckForCondition(opcode)) {
            pc += offs;
            frame_cycles += 4;
        }
        break;
    }
    case 0x2f: {    // CPL
        debug("0x%04x | 0x%02x | CPL\n", pc-1, opcode);
        a = ~a.Value();
        N = true;
        HC = true;
        break;
    }
    case 0x40 ... 0x75: case 0x77 ... 0x7f: {   // LD r8, r8
        debug("0x%04x | 0x%02x | LD r8, r8\n", pc-1, opcode);
        SetR8((opcode >> 3) & 7, GetR8(opcode & 7));
        break;
    }
    case 0x80 ... 0x87: {   // ADD A, r8
        debug("0x%04x | 0x%02x | ADD A, r8\n", pc-1, opcode);
        u8 operand = GetR8(opcode & 7);
        u8 res = a.Value() + operand;
        Z = (res == 0);
        N = true;
        HC = (operand & 0xf) + (a.Value() & 0xf) > 0xf;
        C = res < a.Value();
        a = res;
        break;
    }
    case 0x88 ... 0x8f: {   // ADC A, r8
        debug("0x%04x | 0x%02x | ADC A, u8\n", pc-1, opcode);
        u8 operand = GetR8(opcode & 7);
        u16 res = u16(a.Value() + operand + C.Value());
        Z = ((res & 0xff) == 0);
        N = false;
        HC = (C.Value()) ? (operand & 0xf) + (a.Value() & 0xf) >= 0xf : (operand & 0xf) + (a.Value() & 0xf) > 0xf;
        C = (res >> 8) & 1;
        a = (res & 0xff);
        break;
    }
    case 0x90 ... 0x97: {    // SUB A, r8
        debug("0x%04x | 0x%02x | SUB A, r8\n", pc-1, opcode);
        u8 operand = GetR8(opcode & 7);
        u8 res = a.Value() - operand;
        Z = (res == 0);
        N = true;
        HC = (operand & 0xf) > (a.Value() & 0xf);
        C = res > a.Value();
        a = res;
        break;
    }
    case 0xa0 ... 0xa7: {   // AND A, r8
        debug("0x%04x | 0x%02x | AND A, r8\n", pc-1, opcode);
        a = a.Value() & GetR8(opcode & 7);
        Z = (a.Value() == 0);
        N = false;
        HC = true;
        C = false;
        break;
    }
    case 0xa8 ... 0xaf: {   // XOR A, r8
        debug("0x%04x | 0x%02x | XOR A, r8\n", pc-1, opcode);
        a = a.Value() ^ GetR8(opcode & 7);
        Z = (a.Value() == 0);
        N = false;
        HC = false;
        C = false;
        break;
    }
    case 0xb0 ... 0xb7: {   // OR A, r8
        debug("0x%04x | 0x%02x | OR A, r8\n", pc-1, opcode);
        a = a.Value() | GetR8(opcode & 7);
        Z = (a.Value() == 0);
        N = false;
        HC = false;
        C = false;
        break; 
    }
    case 0xb8 ... 0xbf: {   // CP A, r8
        debug("0x%04x | 0x%02x | CP A, r8\n", pc-1, opcode);
        u8 operand = GetR8(opcode & 7);
        u8 res = a.Value() - operand;
        Z = (res == 0);
        N = true;
        HC = (operand & 0xf) > (a.Value() & 0xf);
        C = res > a.Value();
        break;
    }
    case 0xc0: case 0xd0: case 0xc8: case 0xd8: case 0xc9: {    // RET Condition
        debug("0x%04x | 0x%02x | RET Cond\n", pc-1, opcode);
        if(CheckForCondition(opcode)) {
            pc = Pop();
            frame_cycles += 4;
        }
        break;
    }
    case 0xc1: case 0xd1: case 0xe1: case 0xf1: { // POP r16
        debug("0x%04x | 0x%02x | POP r16\n", pc-1, opcode);
        SetR16<3>((opcode >> 4) & 3, Pop());
        break;
    }
    case 0xc2: case 0xd2: case 0xca: case 0xda: case 0xc3: {    // JP Condition u16
        debug("0x%04x | 0x%02x | JP Cond u16\n", pc-1, opcode);
        u16 addr = Fetch16();
        if(CheckForCondition(opcode)) {
            pc = addr;
            frame_cycles += 4;
        }
        break;
    }
    case 0xc5: case 0xd5: case 0xe5: case 0xf5: {   // PUSH r16
        debug("0x%04x | 0x%02x | PUSH r16\n", pc-1, opcode);
        Push(GetR16<3>((opcode >> 4) & 3));
        break;
    }
    case 0xc4: case 0xd4: case 0xcc: case 0xdc: case 0xcd: {    // CALL Condition u16
        debug("0x%04x | 0x%02x | CALL Cond u16\n", pc-1, opcode);
        u16 addr = Fetch16();
        if(CheckForCondition(opcode)) {
            Push(pc);
            pc = addr;
            frame_cycles += 4;
        }
        break;
    }
    case 0xc6: {    // ADD A, u8
        debug("0x%04x | 0x%02x | ADD A, u8\n", pc-1, opcode);
        u8 operand = Fetch8();
        u8 res = a.Value() + operand;
        Z = (res == 0);
        N = false;
        HC = (operand & 0xf) + (a.Value() & 0xf) > 0xf;
        C = res < a.Value();
        a = res;
        break;
    }
    case 0xc7: case 0xd7: case 0xe7: case 0xf7: case 0xcf: case 0xdf: case 0xef: case 0xff: {   // RST
        debug("0x%04x | 0x%02x | RST\n", pc-1, opcode);
        Push(pc);
        pc = opcode & 0x38;
        frame_cycles += 4;
        break;
    }
    case 0xcb: {    // CB Prefix
        opcode = Fetch8();
        switch(opcode) {
        case 0x00 ... 0x07: {   // RLC r8
            debug("0x%04x | 0x%02x | RLC r8\n", pc-1, opcode);
            u8 r8 = GetR8(opcode & 7);
            u8 temp = r8;
            r8 = Helpers::rotl<u8>(r8, 1);
            Z = (r8 == 0);
            N = false;
            HC = false;
            C = (temp & 0x80) >> 7;
            SetR8(opcode & 7, r8);
            break; 
        }
        case 0x10 ... 0x17: {   // RL r8
            debug("0x%04x | 0x%02x | RL r8\n", pc-1, opcode);
            u8 r8 = GetR8(opcode & 7);
            u8 temp = r8;
            r8 = (r8 << 1) | C.Value();
            Z = (r8 == 0);
            N = false;
            HC = false;
            C = (temp & 0x80) >> 7;
            SetR8(opcode & 7, r8);
            break; 
        }
        case 0x18 ... 0x1f: {   // RR r8
            debug("0x%04x | 0x%02x | RR r8\n", pc-1, opcode);
            u8 r8 = GetR8(opcode & 7);
            u8 temp = r8;
            u8 carry = C.Value();
            r8 = (r8 >> 1) | (carry << 7);
            Z = (r8 == 0);
            N = false;
            HC = false;
            C = temp & 1;
            SetR8(opcode & 7, r8);
            break;
        }
        case 0x20 ... 0x27: {   // // SLA r8
            debug("0x%04x | 0x%02x | SLA r8\n", pc-1, opcode);
            u8 r8 = GetR8(opcode & 7);
            C = (r8 >> 7) & 1;
            r8 <<= 1;
            Z = (r8 == 0);
            N = false;
            HC = false;
            SetR8(opcode & 7, r8);
        }
        case 0x30 ... 0x37: {   // SWAP r8
            debug("0x%04x | 0x%02x | SWAP r8\n", pc-1, opcode);
            u8 r8 = GetR8(opcode & 7);
            r8 = Helpers::rotr<u8>(r8, 4);
            Z = (r8 == 0);
            N = false;
            HC = false;
            C = false;
            SetR8(opcode & 7, r8);
            break;
        }
        case 0x38 ... 0x3f: {   // SRL r8
            debug("0x%04x | 0x%02x | SRL r8\n", pc-1, opcode);
            u8 r8 = GetR8(opcode & 7);
            C = r8 & 1;
            r8 >>= 1;
            Z = (r8 == 0);
            N = false;
            HC = false;
            SetR8(opcode & 7, r8);
            break;
        }
        case 0x40 ... 0x7f: {   // BIT pos, r8
            debug("0x%04x | 0x%02x | BIT pos, r8\n", pc-1, opcode);
            u8 pos = (opcode >> 3) & 7;
            u8 r8 = GetR8(opcode & 7);
            u8 bit = (r8 >> pos) & 1;
            Z = (bit == 0);
            N = false;
            HC = false;
            break;
        }
        case 0x80 ... 0xbf: {   // RES pos, r8
            debug("0x%04x | 0x%02x | RES pos, r8\n", pc-1, opcode);
            u8 r16 = GetR8(opcode & 7);
            u8 pos = (opcode >> 3) & 7;
            r16 &= ~(1 << pos);
            SetR8(opcode & 7, r16);
            break;
        }

        default:
            Helpers::panic("Unhandled CB opcode 0x{:02X}\n", opcode);
        }
        break;
    }
    case 0xce: {    // ADC A, u8
        debug("0x%04x | 0x%02x | ADC A, u8\n", pc-1, opcode);
        u8 operand = Fetch8();
        u16 res = u16(a.Value() + operand + C.Value());
        Z = ((res & 0xff) == 0);
        N = false;
        HC = (C.Value()) ? (operand & 0xf) + (a.Value() & 0xf) >= 0xf : (operand & 0xf) + (a.Value() & 0xf) > 0xf;
        C = (res >> 8) & 1;
        a = (res & 0xff);
        break;
    }
    case 0xd6: {    // SUB A, u8
        debug("0x%04x | 0x%02x | SUB A, u8\n", pc-1, opcode);
        u8 operand = Fetch8();
        u8 res = a.Value() - operand;
        Z = (res == 0);
        N = false;
        HC = (operand & 0xf) > (a.Value() & 0xf);
        C = res > a.Value();
        a = res;
        break;
    }
    case 0xd9: {    // RETI
        debug("0x%04x | 0x%02x | RETI\n", pc-1, opcode);
        pc = Pop();
        frame_cycles += 4;
        ime = true;
        break;
    }
    case 0xe0: {    // LD (FF00+u8), a
        debug("0x%04x | 0x%02x | LD (FF00+u8), a\n", pc-1, opcode);
        Memory->Write(0xff00 + Fetch8(), a.Value());
        break;
    }
    case 0xe2: {    // LD (FF00+c), a
        debug("0x%04x | 0x%02x | LD (FF00+c), a\n", pc-1, opcode);
        Memory->Write(0xff00 + c.Value(), a.Value());
        break;
    }
    case 0xe6: {    // AND A, u8
        debug("0x%04x | 0x%02x | AND A, u8\n", pc-1, opcode);
        a = a.Value() & Fetch8();
        Z = (a.Value() == 0);
        N = false;
        HC = true;
        C = false;
        break;
    }
    case 0xe9: {    // JP HL
        debug("0x%04x | 0x%02x | JP HL\n", pc-1, opcode);
        pc = hl;
        break;
    }
    case 0xea: {    // LD (u16), a
        debug("0x%04x | 0x%02x | LD (u16), a\n", pc-1, opcode);
        Memory->Write(Fetch16(), a.Value());
        break;
    }
    case 0xee: {    // XOR A, u8
        debug("0x%04x | 0x%02x | XOR A, u8\n", pc-1, opcode);
        a = a.Value() ^ Fetch8();
        Z = (a.Value() == 0);
        N = false;
        HC = false;
        C = false;
        break;
    }
    case 0xf0: {    // LD A, (FF00 + u8)
        debug("0x%04x | 0x%02x | LD A, (FF00 + u8)\n", pc-1, opcode);
        a = Memory->Read(0xff00 + Fetch8());
        break;
    }
    case 0xf3: {    // DI
        debug("0x%04x | 0x%02x | DI\n", pc-1, opcode);
        ime = false;
        break;
    }
    case 0xf9: {    // LD SP, HL
        debug("0x%04x | 0x%02x | LD SP, HL\n", pc-1, opcode);
        sp = hl;
        break;
    }
    case 0xf6: {    // OR A, u8
        debug("0x%04x | 0x%02x | OR A, u8\n", pc-1, opcode);
        a = a.Value() | Fetch8();
        Z = (a.Value() == 0);
        N = false;
        HC = false;
        C = false;
        break;
    }
    case 0xfa: {    // LD A, (u16)
        debug("0x%04x | 0x%02x | LD A, u16\n", pc-1, opcode);
        a = Memory->Read(Fetch16());
        break;
    }
    case 0xfb: {    // DI
        debug("0x%04x | 0x%02x | EI\n", pc-1, opcode);
        ime = true;
        break;
    }
    case 0xfe: {    // CP A, u8
        debug("0x%04x | 0x%02x | CP A, u8\n", pc-1, opcode);
        u8 operand = Fetch8();
        u8 res = a.Value() - operand;
        Z = (res == 0);
        N = true;
        HC = (operand & 0xf) > (a.Value() & 0xf);
        C = res > a.Value();
        break;
    }

    default:
        Helpers::panic("Unhandled opcode 0x{:02X}\n", opcode);
    }

    if(Memory->PPU->VBlankIRQ) Memory->IF |= 1;
    u8 interrupt = Memory->IF & Memory->IE;
    if(interrupt && ime) {    // Handle interrupts
        if(interrupt == 1) {
            Memory->IF &= ~1;
            Push(pc);
            pc = 0x40;
            ime = false;
            frame_cycles += 20;
            Memory->PPU->VBlankIRQ = false;
        }
    }
}