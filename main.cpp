#include <iostream>
#include "Tree.h"

int main() {
    try {
        constexpr size_t arr_size = 4;
        Tree<int, arr_size> tree;
        tree.insert(4);
        std::cout << tree.to_string();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
