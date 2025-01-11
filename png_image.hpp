#ifndef PNG_IMAGE_HPP
#define PNG_IMAGE_HPP

#include <png.h>

#include <array>
#include <ostream>
#include <string>

struct PngImage {
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytepp row_pointers;

    png_uint_32 getWidth() const;
    png_uint_32 getHeight() const;
    int getColor_type() const;
    int getBit_depth() const;
    int getChannels() const;
    int getChannelbytes() const;
    size_t getRowbytes() const;

    PngImage();
    ~PngImage();
    void read_png(const std::string& file_name);
    std::array<uint8_t, 4> getPixel(int x, int y) const;
};

std::ostream& operator<<(std::ostream& os, const PngImage& img);

#endif  // PNG_IMAGE_HPP
