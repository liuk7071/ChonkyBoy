#pragma once
#include <array>
#include "utils.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include "cart.hpp"

class gb {
public:
    // Components
    cart Cart = cart("H:\\Games\\GB\\Tetris (World) (Rev A).gb");
    memory Memory = memory(&Cart);
    cpu Cpu = cpu(&Memory);

    static const int width = 160;
    static const int height = 144;

    void step();
    void runFrame();

    bool isRunning = false;
    int framesPassed = 0;
    std::array <u8, width * height * 4> framebuffer; // 160x144 RGBA framebuffer

    gb() {
        framebuffer.fill (0xFF);
    }
};