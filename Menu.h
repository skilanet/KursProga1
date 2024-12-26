#pragma once
#include "Tree.h"
#include <iostream>
#include <fstream>

template<typename T, size_t arr_size>
class Menu {
public:
    void run();

private:
    Tree<T, arr_size> tree;

    void print_menu();

    void add_element();

    void remove_element();

    void remove_by_value();

    void remove_by_index();

    void print_tree();

    void traverse_tree();

    void clear_tree();

    void save_to_text_file();

    void load_from_text_file();

    void save_to_binary_file();

    void load_from_binary_file();

    void get_by_index();

    void add_by_index();

    void sort_tree();

    void insert_with_save_order();
};

template<typename T, size_t arr_size>
void Menu<T, arr_size>::run() {
    int choice;
    do {
        print_menu();
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: add_element();
                break;
            case 2: remove_element();
                break;
            case 3: print_tree();
                break;
            case 4: add_by_index();
                break;
            case 5: sort_tree();
                break;
            case 6: traverse_tree();
                break;
            case 7: clear_tree();
                break;
            case 8: save_to_text_file();
                break;
            case 9: load_from_text_file();
                break;
            case 10: save_to_binary_file();
                break;
            case 11: load_from_binary_file();
                break;
            case 12: get_by_index();
                break;
            case 13: insert_with_save_order();
                break;
            case 0: std::cout << "Exiting...\n";
                break;
            default: std::cout << "Invalid choice. Please try again.\n";

                std::cout << "\033[2J";
                std::cout << "\033[H";
        }
    } while (choice != 0);
}

template<typename T, size_t arr_size>
void Menu<T, arr_size>::print_menu() {
    std::cout << "\n=== Tree Menu ===\n"
            << "1. Add element\n"
            << "2. Remove element\n"
            << "3. Print tree\n"
            << "4. Add value by index\n"
            << "5. Sort tree\n"
            << "6. Traverse tree\n"
            << "7. Clear tree\n"
            << "8. Save tree to text file\n"
            << "9. Load tree from text file\n"
            << "10. Save tree to binary file\n"
            << "11. Load tree from binary file\n"
            << "12. Get by index\n"
            << "13. Insert with save order\n"
            << "0. Exit\n";
}

template<typename T, size_t arr_size>
void Menu<T, arr_size>::add_element() {
    T value;
    std::cout << "Enter value to add: ";
    std::cin >> value;
    if (tree.insert(value)) {
        std::cout << "Value successfully inserted.\n";
    } else {
        std::cout << "Failed to insert value.\n";
    }
}

template<typename T, size_t arr_size>
void Menu<T, arr_size>::remove_element() {
    int sub_choice;
    std::cout << "\n=== Delete Point Menu ===\n"
            << "1. Delete by Value\n"
            << "2. Delete by Index\n"
            << "0. Back to Main Menu\n"
            << "Enter your choice: ";
    std::cin >> sub_choice;

    switch (sub_choice) {
        case 1: remove_by_value();
            break;
        case 2: remove_by_index();
            break;
        case 0: std::cout << "Returning to Main Menu...\n";
            break;
        default: std::cout << "Invalid choice. Returning to Main Menu.\n";
    }
}

template<typename T, size_t arr_size>
void Menu<T, arr_size>::remove_by_value() {
    T value;
    std::cout << "Enter value to remove: ";
    std::cin >> value;
    if (tree.remove(value)) {
        std::cout << "Value successfully removed.\n";
    } else {
        std::cout << "Value not found.\n";
    }
}

template<typename T, size_t arr_size>
void Menu<T, arr_size>::remove_by_index() {
    size_t index;
    std::cout << "Enter index to remove: ";
    std::cin >> index;
    if (tree.remove_by_index(index)) {
        std::cout << "Element at index " << index << " successfully removed.\n";
    } else {
        std::cout << "Index out of bounds or removal failed.\n";
    }
}

template<typename T, size_t arr_size>
void Menu<T, arr_size>::print_tree() {
    std::cout << "Tree contents: \n";
    tree.print_tree();
}

template<typename T, size_t arr_size>
void Menu<T, arr_size>::traverse_tree() {
    std::cout << "Tree traversal: ";
    tree.in_order_traversal(true);
}

template<typename T, size_t arr_size>
void Menu<T, arr_size>::clear_tree() {
    tree.clear();
    std::cout << "Tree cleared.\n";
}

template<typename T, size_t arr_size>
void Menu<T, arr_size>::save_to_text_file() {
    std::ofstream file("tree.txt");
    if (file) {
        file << tree;
        std::cout << "Tree saved to text file.\n";
    } else {
        std::cout << "Failed to open file.\n";
    }
}

template<typename T, size_t arr_size>
void Menu<T, arr_size>::load_from_text_file() {
    std::ifstream file("tree.txt");
    if (file) {
        file >> tree;
        std::cout << "Tree loaded from text file.\n";
    } else {
        std::cout << "Failed to open file.\n";
    }
}

template<typename T, size_t arr_size>
void Menu<T, arr_size>::save_to_binary_file() {
    if (std::ofstream file("tree.bin", std::ios::binary); file) {
        tree.save_to_binary_file(file);
        std::cout << "Tree saved to binary file.\n";
    } else {
        std::cout << "Failed to open file.\n";
    }
}

template<typename T, size_t arr_size>
void Menu<T, arr_size>::load_from_binary_file() {
    if (std::ifstream file("tree.bin", std::ios::binary); file) {
        tree.load_from_binary_file(file);
        std::cout << "Tree loaded from binary file.\n";
    } else {
        std::cout << "Failed to open file.\n";
    }
}

template<typename T, size_t arr_size>
void Menu<T, arr_size>::get_by_index() {
    size_t index;
    std::cout << "Enter index: ";
    std::cin >> index;
    std::cout << tree.get_by_index(index) << std::endl;
}

template<typename T, size_t arr_size>
void Menu<T, arr_size>::add_by_index() {
    std::cout << "Enter index to add: ";
    int index;
    std::cin >> index;
    std::cout << "Enter value to add: ";
    T value;
    std::cin >> value;
    if (tree.insert_by_index(index, value)) {
        std::cout << "Value successfully added.\n";
    } else {
        std::cout << "Value not found.\n";
    }
}

template<typename T, size_t arr_size>
void Menu<T, arr_size>::sort_tree() {
    std::cout << "Tree sorting...\n";
    if (tree.sort()) {
        std::cout << "Tree successfully sorted.\n";
    } else {
        std::cout << "Tree not sorted.\n";
    }
}

template<typename T, size_t arr_size>
void Menu<T, arr_size>::insert_with_save_order() {
    T value;
    std::cout << "Enter value to add: ";
    std::cin >> value;
    tree.insert_with_order_save(value);
}
