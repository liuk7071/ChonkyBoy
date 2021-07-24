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
    u8 Read(u16 addr);

    u8 wram[4096];
};