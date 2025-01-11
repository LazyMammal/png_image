#include "png_image.hpp"

#include <iostream>
#include <string>

int main(int argc, char **argv) {
    std::string filename = argc > 1 ? std::string(argv[1]) : "maze1.png";
    std::cout << filename << std::endl;

    PngImage png(filename);
    std::cout << png << std::endl;

    char symbols[] = {' ', '\'', '.', ':'};

    for (int y = 0; y < 100 && y < png.height - 1; y += 2) {
        for (int x = 0; x < 200 && x < png.width; x++) {
            auto pixelUp = png.data[y * png.width + x];
            auto pixelDn = png.data[(y + 1) * png.width + x];
            int code = 0;
            code += pixelUp.r > 0 ? 1 : 0;
            code += pixelDn.r > 0 ? 2 : 0;
            std::cout << symbols[code];
        }
        std::cout << std::endl;
    }
}
