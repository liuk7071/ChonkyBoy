#pragma once
#include <array>
#include "utils.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include "cart.hpp"
#include "ppu.hpp"

class gb {
public:
    // Components
    //cart Cart = cart("H:\\Games\\GB\\Tetris (World) (Rev A).gb");
    cart Cart = cart("C:\\Users\\zacse\\Downloads\\06-ld r,r.gb");
    //cart Cart = cart("C:\\Users\\zacse\\Downloads\\09-op r,r.gb");
    //cart Cart = cart("C:\\Users\\zacse\\Downloads\\07-jr,jp,call,ret,rst.gb");
    ppu Ppu = ppu();
    memory Memory = memory(&Cart, &Ppu);
    cpu Cpu = cpu(&Memory);

    static const int width = 160;
    static const int height = 144;

    void step();
    void runFrame();

    bool isRunning = false;
    int framesPassed = 0;
};