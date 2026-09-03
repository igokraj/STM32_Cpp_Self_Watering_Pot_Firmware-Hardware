#pragma once
#include "i2c.h"

// Monochrome 128x64 OLED (SSD1306) driven over I2C.
// Drawing goes into a RAM framebuffer; flush() pushes the whole frame to the panel.
class Display {
  private:
    I2C_HandleTypeDef *i2c_;
    uint8_t address_;       // 8-bit address (7-bit address already shifted left)
    uint8_t buffer_[1024];  // 8 pages x 128 columns, one byte = 8 vertically stacked pixels

    void command(uint8_t cmd);

  public:
    static const uint8_t Width = 128;
    static const uint8_t Height = 64;

    // Most SSD1306 modules answer at 7-bit address 0x3C, some at 0x3D
    Display(I2C_HandleTypeDef *i2c, uint8_t address7bit = 0x3C)
        : i2c_(i2c), address_(static_cast<uint8_t>(address7bit << 1)), buffer_{} {}

    void init();
    void clear();
    void pixel(uint8_t x, uint8_t y, bool on = true);
    void hline(uint8_t x, uint8_t y, uint8_t width);
    void text(uint8_t x, uint8_t y, const char *str);
    void bitmap(uint8_t x, uint8_t y, const uint8_t *bmp, uint8_t w, uint8_t h);
    void flush();
};

// 32x32 flower-pot icon, row-major, 4 bytes per row (MSB = leftmost pixel)
extern const uint8_t IconPlant[128];
