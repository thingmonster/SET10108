#include <iostream>
#include <FreeImagePlus.h>

int main() {
    fipImage img;
    img.load("foobar.png");

    std::cout << img.getWidth() << " " << img.getHeight() << std::endl;
    std::cout << img.isGrayscale() << std::endl;
    std::cout << img.getBitsPerPixel() << std::endl;

    return 0;
}
