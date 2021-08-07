#pragma once
#include <array>
#include <sstream>
#include "utils.hpp"
#include "cart.hpp"
#include "BitField.hpp"

class memory {
public:
    memory(cart* cartptr);
    cart* Cart;
    
    void Write(u16 addr, u8 data);
    void Write16(u16 addr, u16 data);
    u8 Read(u16 addr);
    u16 Read16(u16 addr);

    u8 wram[8192];
    u8 hram[127];
    u8 IE;
    u8 IF;
    u8 TAC;
};