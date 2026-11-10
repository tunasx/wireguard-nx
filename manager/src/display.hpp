#pragma once

#include <switch.h>
#include <string>
#include <vector>
#include <stb_truetype.h>

// UTF-8 diagnostic console using the console's shared fonts, including CJK.
// No Nintendo font data is redistributed with this application.
class ManagerDisplay {
  public:
    void initialize();
    void shutdown();
    void print(const char* format, ...) __attribute__((format(printf, 2, 3)));
    void scroll(int rows);
    void update();

  private:
    static constexpr int Width = 1280;
    static constexpr int Height = 720;
    static constexpr int LineHeight = 24;
    static constexpr int VisibleRows = 28;
    Framebuffer framebuffer{};
    stbtt_fontinfo fonts[2]{};
    bool hasChinese = false;
    bool dirty = true;
    int firstRow = 0;
    std::string text;
    std::vector<u32> pixels;
    void render();
};
