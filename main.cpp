#include <fstream>
#include <iostream>
#include "Menu.h"

int main() {
    try {
        constexpr int size = 5;
        Menu<int, size> menu;
        menu.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
