#pragma once

#include <cstdint>
#include <string>

class Window{
public:
    static bool init(const uint32_t width, const uint32_t height, const std::string& title);
    static bool isShouldClose();
    static void PollEvents();
    static void SwapBuffers();
    static void term();
};