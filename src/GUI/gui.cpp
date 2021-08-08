#include <filesystem>          // For std::filesystem::path
#include "fmt/format.h"        // For fmt::print
#include "tinyfiledialogs.h"   // For file explorer
#include "gui.hpp"
#include "gb.hpp"

GUI::GUI (gb& emulator) : window(sf::VideoMode(800, 600), "ChonkyBoy"), emulator(emulator) {
    window.setFramerateLimit(60); // cap FPS to 60
    ImGui::SFML::Init(window);    // Init Imgui-SFML
    display.create (gb::width, gb::height);

    auto& io = ImGui::GetIO();  // Set some ImGui options
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable navigation with keyboard
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Configure viewport feature
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
}

void GUI::update() {
    sf::Event event;
    
    while (window.pollEvent(event)) { // Poll events
        ImGui::SFML::ProcessEvent(event);
        if (event.type == sf::Event::Closed)
            window.close();
    }

    if (emulator.isRunning)
        emulator.runFrame();

    ImGui::SFML::Update(window, deltaClock.restart()); // Update imgui-sfml

    showMenuBar(); // Render GUI stuff
    showDisplay();
    ImGui::ShowDemoWindow();

    drawGUI();
}

void GUI::showMenuBar() {
    static const char* romTypes[] = { "*.bin", "*.rom" }; // Some generic filetypes for ROMs, configure as you want
    
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu ("File")) { // Show file selection dialog if open ROM button is pressed
            if (ImGui::MenuItem ("Open ROM", nullptr)) {
                auto file = tinyfd_openFileDialog(
                    "Choose a ROM", // File explorer window title
                    "",             // Default directory
                    2,              // Amount of file types
                    romTypes,       // Array of file types
                    "ROMs",         // File type description in file explorer window
                    0);

                if (file != nullptr) {  // Check if file dialog was canceled
                    const auto path = std::filesystem::path (file);
                    fmt::print ("Opened file {}\n", path.string());
                }
            }
            
            if (ImGui::MenuItem ("Save state", nullptr))
                fmt::print ("Save state");

            if (ImGui::MenuItem ("Load state", nullptr))
                fmt::print ("Load state");

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu ("Emulation")) {
            if (ImGui::MenuItem ("Step", nullptr))
                emulator.step();
            ImGui::MenuItem ("Run", nullptr, &emulator.isRunning);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

// TODO: Optimize this
// This is fairly slow, so for best measure if you don't want presentation to bottleneck emulation too much
// You should have the emulator on a separate thread from the GUI
void GUI::showDisplay() {
    bool show = false;
    ImGui::Begin (("Display"), &show, ImGuiWindowFlags_NoTitleBar);
    const auto size = ImGui::GetContentRegionAvail();
    const auto scale_x = size.x / gb::width;
    const auto scale_y = size.y / gb::height;
    const auto scale = scale_x < scale_y ? scale_x : scale_y;

    display.update (emulator.Ppu.framebuffer.data()); // Present the buffer that's not being currently written to
    sf::Sprite sprite (display);
    sprite.setScale (scale, scale);
    ImVec2 image_size(scale * gb::width, scale * gb::height);
    ImVec2 center((ImGui::GetWindowSize().x - image_size.x) / 2, (ImGui::GetWindowSize().y - image_size.y) / 2);
    ImGui::SetCursorPos(center);
    ImGui::Image (sprite);
    ImGui::End();
    
}

void GUI::drawGUI() {
    window.clear (sf::Color(115, 140, 153, 255)); // Clear window with turquoise
    ImGui::SFML::Render(window); // Render ImGui contents
    window.display(); // Display ImGui contents
}