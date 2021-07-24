#pragma once
#include <array>
#include <utils.hpp>

class cart {
public:
    cart(const char* dir);
    FILE* data;

    u8 read(u16 addr);
};