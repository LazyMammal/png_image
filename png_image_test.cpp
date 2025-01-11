#include "png_image.hpp"

#include <iostream>

int main() {
    PngImage png;
    png.read_png("maze1.png");
    std::cout << png << std::endl;
    char symbols[] = {' ', '\'', '.', ':'};

    for (int y = 0; y < 100 && y < png.getHeight(); y += 2) {
        for (int x = 0; x < 200 && x < png.getWidth(); x++) {
            int code = 0;
            code += png.getPixel(x, y)[0] > 0 ? 1 : 0;
            code += png.getPixel(x, y + 1)[0] > 0 ? 2 : 0;
            std::cout << symbols[code];
        }
        std::cout << std::endl;
    }
}
