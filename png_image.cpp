#include "png_image.hpp"

#include <png.h>

#include <array>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

PngImage::PngImage() : png_ptr(nullptr), info_ptr(nullptr), row_pointers(nullptr) {}
PngImage::~PngImage() {
    // assert: row_pointers alloc and free are managed by libpng (confirmed with Valgrind)
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}

png_uint_32 PngImage::getWidth() const { return png_get_image_width(png_ptr, info_ptr); }
png_uint_32 PngImage::getHeight() const { return png_get_image_height(png_ptr, info_ptr); }
int PngImage::getColor_type() const { return png_get_color_type(png_ptr, info_ptr); }
int PngImage::getBit_depth() const { return png_get_bit_depth(png_ptr, info_ptr); }
int PngImage::getChannels() const { return png_get_channels(png_ptr, info_ptr); }
int PngImage::getChannelbytes() const { return getBit_depth() / 8; }
size_t PngImage::getRowbytes() const { return png_get_rowbytes(png_ptr, info_ptr); }

void PngImage::read_png(const std::string& file_name) {
    FILE* fp = fopen(file_name.c_str(), "rb");
    if (!fp) throw std::runtime_error("Failed to open file");

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) throw std::runtime_error("Failed to create read struct");

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        throw std::runtime_error("Failed to create info struct");
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        throw std::runtime_error("Error reading PNG file");
    }

    png_init_io(png_ptr, fp);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* OPTIONAL: expand low bit depth images */
    if (getColor_type() == PNG_COLOR_TYPE_PALETTE && getBit_depth() <= 8) png_set_expand(png_ptr);
    if (getColor_type() == PNG_COLOR_TYPE_GRAY && getBit_depth() < 8) png_set_expand(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_expand(png_ptr);
    /* END */

    if (getRowbytes() != getWidth() * getChannels() * getChannelbytes()) {
        throw std::runtime_error("Image rowbytes size mismatch");
    }

    row_pointers = png_get_rows(png_ptr, info_ptr);

    fclose(fp);
}

std::array<uint8_t, 4> PngImage::getPixel(int x, int y) const {
    if (x < 0 || x >= getWidth() || y < 0 || y >= getHeight()) {
        throw std::out_of_range("Pixel coordinates out of bounds");
    }

    std::array<uint8_t, 4> pixel = {0, 0, 0, 255};  // Default to black, fully opaque
    png_bytep row = row_pointers[y];

    const int offset = x * getChannels() * getChannelbytes();

    switch (getChannels()) {
        case 2:  // PNG_COLOR_TYPE_GRAY_ALPHA
            pixel[3] = row[offset + 1];
        case 1:  // PNG_COLOR_TYPE_GRAY(_ALPHA)
            pixel[0] = pixel[1] = pixel[2] = row[offset];
            break;
        case 4:  // PNG_COLOR_TYPE_RGB_ALPHA
            pixel[3] = row[offset + 3];
        case 3:  // PNG_COLOR_TYPE_RGB(_ALPHA)
            pixel[0] = row[offset];
            pixel[1] = row[offset + 1];
            pixel[2] = row[offset + 2];
            break;
        default:
            throw std::runtime_error("Unsupported channels count");
    }

    return pixel;
}

std::ostream& operator<<(std::ostream& os, const PngImage& img) {
    os << "PngImage {\n"
       << "  width: " << img.getWidth() << "\n"
       << "  height: " << img.getHeight() << "\n"
       << "  color_type: " << img.getColor_type() << " (";

    switch (img.getColor_type()) {
        case PNG_COLOR_TYPE_GRAY:
            os << "Grayscale";
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            os << "Grayscale with Alpha";
            break;
        case PNG_COLOR_TYPE_PALETTE:
            os << "Palette";
            break;
        case PNG_COLOR_TYPE_RGB:
            os << "RGB";
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            os << "RGBA";
            break;
        default:
            os << "Unknown";
            break;
    }

    os << ")\n"
       << "  bit_depth: " << img.getBit_depth() << "\n"
       << "  channels: " << img.getChannels() << "\n"
       << "  bytes per channel: " << img.getChannelbytes() << "\n"
       << "}";

    return os;
}
