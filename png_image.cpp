#include "png_image.hpp"

#include <png.h>

PngImage::PngImage(const std::string& filename) {
    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        throw std::runtime_error("Failed to create PNG read struct.");
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        fclose(fp);
        throw std::runtime_error("Failed to create PNG info struct.");
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fp);
        throw std::runtime_error("Error during PNG reading.");
    }

    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);  // Convert palette images to RGB
        color_type = PNG_COLOR_TYPE_RGB;  // Update color type
    }

    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(png_ptr);  // Expand grayscale images with less than 8 bits
        bit_depth = 8;
    }

    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png_ptr);     // Convert grayscale images to RGB
        color_type = PNG_COLOR_TYPE_RGB;  // Update color type
    }

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);  // Handle transparency
    }

    if (color_type == PNG_COLOR_TYPE_RGB) {
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);  // Add alpha channel
        color_type = PNG_COLOR_TYPE_RGB_ALPHA;            // Update color type
    }

    png_read_update_info(png_ptr, info_ptr);  // Update info structure after transformations

    data.resize(width * height);     // RGBA type already has 4 bytes per pixel
    png_bytep row_pointers[height];  // pointers to raw image data rows
    for (int y = 0; y < height; y++) {
        row_pointers[y] = (png_bytep)(&data[y * width]);  // start of data row
    }

    png_read_image(png_ptr, row_pointers);               // Read the image data into the RGBA buffer
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);  // cleanup
    fclose(fp);
}

std::ostream& operator<<(std::ostream& os, const PngImage& img) {
    return os << "PngImage " << img.width << "x" << img.height;
}
