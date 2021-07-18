#pragma once
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "gb.hpp"

class GUI {
    sf::RenderWindow window;
    sf::Clock deltaClock;
    sf::Texture display;
    gb& emulator;

public:
    GUI (gb& emulator);

    void update(); // Update the GUI
    bool isOpen() { return window.isOpen(); } // Shows if the GUI window has been closed or not

private:
    void showMenuBar();
    void showDisplay();
    void drawGUI();
};