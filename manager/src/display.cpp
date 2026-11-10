#define STB_TRUETYPE_IMPLEMENTATION
#include "display.hpp"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>

void ManagerDisplay::initialize() {
    Result rc = plInitialize(hosversionAtLeast(16, 0, 0) ? PlServiceType_User : PlServiceType_System);
    if (R_FAILED(rc))
        diagAbortWithResult(rc);
    PlFontData standard{}, chinese{};
    rc = plGetSharedFontByType(&standard, PlSharedFontType_Standard);
    if (R_FAILED(rc))
        diagAbortWithResult(rc);
    if (!stbtt_InitFont(&fonts[0], static_cast<const unsigned char*>(standard.address), 0))
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_BadInput));
    hasChinese = R_SUCCEEDED(plGetSharedFontByType(&chinese, PlSharedFontType_ChineseSimplified)) &&
                 stbtt_InitFont(&fonts[1], static_cast<const unsigned char*>(chinese.address), 0);
    rc = framebufferCreate(&framebuffer, nwindowGetDefault(), Width, Height, PIXEL_FORMAT_RGBA_8888, 2);
    if (R_FAILED(rc))
        diagAbortWithResult(rc);
    rc = framebufferMakeLinear(&framebuffer);
    if (R_FAILED(rc))
        diagAbortWithResult(rc);
    pixels.resize(Width * Height, 0xff000000);
}

void ManagerDisplay::shutdown() {
    framebufferClose(&framebuffer);
    plExit();
}

void ManagerDisplay::print(const char* format, ...) {
    char buffer[4096];
    va_list args;
    va_start(args, format);
    std::vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    // Keep nxlink diagnostics available while drawing UTF-8 locally.
    std::fputs(buffer, stdout);
    text += buffer;
    if (text.size() > 65536) {
        const auto newline = text.find('\n', text.size() - 32768);
        if (newline != std::string::npos)
            text.erase(0, newline + 1);
    }
    // Follow new diagnostics, like a terminal; users can scroll back with Up.
    firstRow = 65536;
    dirty = true;
}

void ManagerDisplay::scroll(int rows) {
    firstRow = std::max(0, firstRow + rows);
    dirty = true;
}

void ManagerDisplay::render() {
    struct Glyph {
        int codepoint;
        int x;
        int row;
        int font;
        float scale;
    };
    std::vector<Glyph> glyphs;
    int x = 20, row = 0;
    const auto* cursor = reinterpret_cast<const unsigned char*>(text.c_str());
    while (*cursor) {
        unsigned int codepoint = *cursor++;
        if (codepoint >= 0x80) {
            const int count = codepoint >= 0xf0 ? 3 : codepoint >= 0xe0 ? 2 : 1;
            codepoint &= (1u << (6 - count)) - 1;
            for (int i = 0; i < count; ++i) {
                if ((*cursor & 0xc0) != 0x80) {
                    codepoint = '?';
                    break;
                }
                codepoint = (codepoint << 6) | (*cursor++ & 0x3f);
            }
            if (codepoint > 0x10ffff || (codepoint >= 0xd800 && codepoint <= 0xdfff))
                codepoint = '?';
        }
        if (codepoint == '\n') {
            x = 20;
            ++row;
            continue;
        }
        if (codepoint < 32)
            continue;
        int font = 0;
        if (hasChinese && !stbtt_FindGlyphIndex(&fonts[0], codepoint))
            font = 1;
        const float scale = stbtt_ScaleForPixelHeight(&fonts[font], 22);
        int advance = 0;
        stbtt_GetCodepointHMetrics(&fonts[font], codepoint, &advance, nullptr);
        const int width = std::max(1, static_cast<int>(advance * scale + 0.5f));
        if (x + width > Width - 20) {
            x = 20;
            ++row;
        }
        glyphs.push_back({static_cast<int>(codepoint), x, row, font, scale});
        x += width;
    }
    firstRow = std::min(firstRow, std::max(0, row + 1 - VisibleRows));
    std::fill(pixels.begin(), pixels.end(), 0xff000000);
    for (const auto& glyph : glyphs) {
        if (glyph.row < firstRow || glyph.row >= firstRow + VisibleRows)
            continue;
        int width, height, xOffset, yOffset;
        auto* bitmap = stbtt_GetCodepointBitmap(&fonts[glyph.font], 0, glyph.scale, glyph.codepoint, &width, &height, &xOffset, &yOffset);
        if (!bitmap)
            continue;
        const int baseline = 36 + (glyph.row - firstRow) * LineHeight;
        for (int y = 0; y < height; ++y) {
            for (int xx = 0; xx < width; ++xx) {
                const int px = glyph.x + xOffset + xx, py = baseline + yOffset + y;
                if (px < 0 || px >= Width || py < 0 || py >= Height)
                    continue;
                const u32 shade = bitmap[y * width + xx];
                pixels[py * Width + px] = 0xff000000 | shade | (shade << 8) | (shade << 16);
            }
        }
        stbtt_FreeBitmap(bitmap, nullptr);
    }
    dirty = false;
}

void ManagerDisplay::update() {
    if (dirty)
        render();
    u32 stride = 0;
    auto* buffer = static_cast<unsigned char*>(framebufferBegin(&framebuffer, &stride));
    for (int y = 0; y < Height; ++y)
        std::memcpy(buffer + y * stride, pixels.data() + y * Width, Width * sizeof(u32));
    framebufferEnd(&framebuffer);
}
