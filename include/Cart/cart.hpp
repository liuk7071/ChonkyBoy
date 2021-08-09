#pragma once
#include <array>
#include <utils.hpp>

class cart {
public:
    cart(const char* dir);
    std::vector<u8> rom;

    u8 read(u16 addr);
};