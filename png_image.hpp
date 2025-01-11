#ifndef PNG_IMAGE_HPP
#define PNG_IMAGE_HPP

#include <png.h>

#include <ostream>
#include <string>
#include <vector>

struct RGBA {
    uint8_t r, g, b, a;
};

struct PngImage {
    int width, height;
    std::vector<RGBA> data;
    PngImage(const std::string& filename);
};

std::ostream& operator<<(std::ostream& os, const PngImage& img);

#endif  // PNG_IMAGE_HPP
