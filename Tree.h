#pragma once
#include <iostream>
#include "Nodes.h"


/**
 * @brief Класс дерево, в нём реализованы все метода для работы
 * @tparam T Тип хранимых данных
 * @tparam arr_size Размер массива в конечной вершине
 */
template<typename T, int arr_size>
class Tree final {
    std::shared_ptr<TreeNode<T>> root;

    void traverse(const std::shared_ptr<TreeNode<T>> &root, const std::function<void(std::shared_ptr<TreeNode<T>>)> &func);

    bool insert_helper(std::shared_ptr<TreeNode<T>> &node, const T &element);

    bool insert_helper(std::shared_ptr<TreeNode<T>> &node, size_t index, const T &element);

    bool remove_helper(std::shared_ptr<TreeNode<T>> &node, size_t index);

    size_t count_leaf_nodes(const std::shared_ptr<TreeNode<T>> &node) const;

    void distribute_elements(const std::shared_ptr<TreeNode<T>> &node,
                             typename std::vector<T>::iterator &it,
                             size_t elements_per_leaf,
                             size_t &remaining_elements);

    std::vector<T> get_all_elements();

    /**
     * @brief Функция для сохранения дерева в текстовый файл работает через оператор '<<'
     * реализация написана в теле класса из-за особенностей реализации
     * @param os Поток вывода данных
     * @param tree Указатель на класс дерева
     * @return Возвращает поток данных для сохранения в файл
     */
    friend std::ostream &operator<<(std::ostream &os, Tree &tree) {
        if (!tree.root) {
            os << "Empty tree\n";
            return os;
        }
        tree.traverse(tree.root, [&](const std::shared_ptr<TreeNode<T>> &node) {
            if (node->get_type() == TYPE::LEAF) {
                auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(node);
                os << "LeafNode: ";
                for (size_t i = 0; i < leaf->get_size(); ++i) {
                    os << leaf->get_element_at(i) << " ";
                }
                os << "\n";
            } else if (node->get_type() == TYPE::INTERMEDIATE) {
                os << "IntermediateNode\n";
            }
        });
        return os;
    }

    /**
     * @brief Функция для чтения дерева из текстового файла
     * @param is Поток входных данных
     * @param tree Указатель на дерево
     * @return Поток данных для вывода
     */
    friend std::istream &operator>>(std::istream &is, Tree &tree) {
        tree.root = nullptr;
        std::string line;

        std::vector<T> elements;
        while (std::getline(is, line)) {
            std::istringstream iss(line);
            std::string type;
            iss >> type;
            if (type == "LeafNode:") {
                T element;
                while (iss >> element) {
                    tree.insert(element);
                }
            }
        }
        for (size_t i = 0; i < elements.size(); ++i) {
            std::cout << elements[i] << " ";
        }
        return is;
    }

    void clear_with_struct();

    T get_by_index_helper(std::shared_ptr<TreeNode<T>> node, size_t index);

    bool isTreeSorted = false;

    bool insert_with_order_helper(std::shared_ptr<TreeNode<T>> &node, const T &element);

public:
    Tree(): root(nullptr) {
    }

    void in_order_traversal(bool);

    ~Tree() = default;

    bool insert(const T &element);

    bool remove(const T &element);

    T get_by_index(size_t index);

    T operator[](int index);

    bool sort();

    void save_to_binary_file(std::ofstream &ofs);

    void load_from_binary_file(std::ifstream &ifs);

    void print_helper();

    void print_tree() {
        print_helper();
    }

    void clear() { root = nullptr; }

    bool insert_by_index(size_t index, const T &element);

    bool remove_by_index(size_t index);

    bool insert_with_order_save(T element);

};

/**
 * @brief Базовая функция для работы с деревом
 * @param root Указатель на вершину дерева
 * @param func Указатель на функцию для того, чтобы её можно было переиспользовать
 */
template<typename T, int arr_size>
void Tree<T, arr_size>::traverse(const std::shared_ptr<TreeNode<T>> &root,
                                 const std::function<void(std::shared_ptr<TreeNode<T>>)> &func) {
    if (root == nullptr) return;
    func(root);
    if (root->get_type() == TYPE::INTERMEDIATE) {
        auto intermediate = std::dynamic_pointer_cast<IntermediateNode<T, arr_size> >(root);
        if (intermediate->get_left_node()) traverse(intermediate->get_left_node(), func);
        if (intermediate->get_right_node()) traverse(intermediate->get_right_node(), func);
    }
}

/**
 * @brief Функция для рекурсивного добавления элементов в дерево
 * эта функция сбалансированною добавляет элемент в дерево, так как иначе добавление происходи странно
 * также это не требует дополнительного балансирования дерева
 * @param node Указатель на вершину дерева
 * @param element Элемент который необходимо добавить
 * @return true если добавление прошло успешно
 * @return false если возникли какие-то ошибки при добавлении
 */
template<typename T, int arr_size>
bool Tree<T, arr_size>::insert_helper(std::shared_ptr<TreeNode<T>> &node, const T &element) {
    if (!node) {
        node = std::make_shared<LeafNode<T, arr_size> >();
        auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(node);
        leaf->add_element(element);
        return true;
    }

    if (node->get_type() == TYPE::LEAF) {
        auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(node);

        if (leaf->add_element(element)) {
            return true;
        }

        auto new_left_node = std::make_shared<LeafNode<T, arr_size> >();
        auto new_right_node = std::make_shared<LeafNode<T, arr_size> >();

        int mid = arr_size / 2;

        for (size_t i = 0; i < arr_size; ++i) {
            if (i < mid) new_left_node->add_element(leaf->get_element_at(i));
            else new_right_node->add_element(leaf->get_element_at(i));
        }

        new_right_node->add_element(element);

        auto new_intermediate = std::make_shared<IntermediateNode<T, arr_size> >();
        new_intermediate->set_left_node(new_left_node);
        new_intermediate->set_right_node(new_right_node);

        node = new_intermediate;
        return true;
    }

    if (node->get_type() == TYPE::INTERMEDIATE) {
        auto intermediate = std::dynamic_pointer_cast<IntermediateNode<T, arr_size> >(node);

        if (!insert_helper(intermediate->get_right_node(), element)) {
            auto new_intermediate = std::make_shared<IntermediateNode<T, arr_size> >();
            auto new_leaf = std::make_shared<LeafNode<T, arr_size> >();
            new_leaf->add_element(element);

            new_intermediate->set_left_node(intermediate);
            new_intermediate->set_right_node(new_leaf);

            node = new_intermediate;
        }
        return true;
    }
    return false;
}

template<typename T, int arr_size>
bool Tree<T, arr_size>::insert_helper(std::shared_ptr<TreeNode<T>> &node, size_t index, const T &element) {
    if (!node) {
        throw std::out_of_range("Index out of range");
    }

    if (node->get_type() == TYPE::LEAF) {
        if (auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size>>(node); !leaf->insert_by_index(index, element)) {
            const size_t mid = arr_size / 2;

            auto new_left_node = std::make_shared<LeafNode<T, arr_size>>();
            auto new_right_node = std::make_shared<LeafNode<T, arr_size>>();

            for (size_t i = 0; i < mid; ++i) {
                new_left_node->add_element(leaf->get_element_at(i));
            }
            for (size_t i = mid; i < leaf->get_size(); ++i) {
                new_right_node->add_element(leaf->get_element_at(i));
            }

            if (index < mid) {
                new_left_node->insert_by_index(index, element);
            } else {
                new_right_node->insert_by_index(index - mid, element);
            }

            auto intermediate = std::make_shared<IntermediateNode<T, arr_size>>();
            intermediate->set_left_node(new_left_node);
            intermediate->set_right_node(new_right_node);

            node = intermediate;
        }
        return true;
    }

    if (node->get_type() == TYPE::INTERMEDIATE) {
        auto intermediate = std::dynamic_pointer_cast<IntermediateNode<T, arr_size>>(node);

        const size_t left_size = intermediate->get_left_node() ? intermediate->get_left_node()->get_size() : 0;

        if (index < left_size) {
            return insert_helper(intermediate->get_left_node(), index, element);
        }
        index -= left_size;
        return insert_helper(intermediate->get_right_node(), index, element);
    }

    return false;
}

template<typename T, int arr_size>
bool Tree<T, arr_size>::remove_helper(std::shared_ptr<TreeNode<T>> &node, const size_t index) {
    if (!node) {
        throw std::out_of_range("Index out of bounds");
    }

    if (node->get_type() == TYPE::LEAF) {
        if (auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size>>(node); index < leaf->get_size()) {
            leaf->remove_by_index(index);

            if (leaf->get_size() == 0) {
                node = nullptr;
            }
            return true;
        }
        throw std::out_of_range("Index out of bounds");
    }

    if (node->get_type() == TYPE::INTERMEDIATE) {
        auto intermediate = std::dynamic_pointer_cast<IntermediateNode<T, arr_size>>(node);

        if (const size_t left_size = intermediate->get_left_node() ? intermediate->get_left_node()->get_size() : 0;
            index < left_size) {
            if (remove_helper(intermediate->get_left_node(), index)) {
                if (!intermediate->get_left_node() && intermediate->get_right_node()) {
                    if (auto right_leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size>>(intermediate->get_right_node())) {
                        auto new_leaf = std::make_shared<LeafNode<T, arr_size>>();
                        new_leaf->add_elements(right_leaf->get_data());
                        node = new_leaf;
                    }
                }
            }
        } else {
            if (remove_helper(intermediate->get_right_node(), index - left_size)) {
                if (intermediate->get_left_node() && !intermediate->get_right_node()) {
                    if (auto left_leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size>>(intermediate->get_left_node())) {
                        auto new_leaf = std::make_shared<LeafNode<T, arr_size>>();
                        new_leaf->add_elements(left_leaf->get_data());
                        node = new_leaf;
                    }
                }
            }
        }

        if (!intermediate->get_left_node() && !intermediate->get_right_node()) {
            node = std::make_shared<LeafNode<T, arr_size>>();
        }

        return true;
    }

    return false;
}


/**
 * @brief Функция которая используется для вызова функции bool Tree<T, arr_size>::insert_helper(std::shared_ptr<TreeNode<T>> &node, const T &element)
 * @param element Элемент который необходимо добавить
 * @return true если добавление прошло успешно
 * @return false если возникли какие-то ошибки при добавлении
 */
template<typename T, int arr_size>
bool Tree<T, arr_size>::insert(const T &element) {
    return insert_helper(root, element);
}

template<typename T, int arr_size>
bool Tree<T, arr_size>::remove(const T &element) {
    traverse(root, [&](const std::shared_ptr<TreeNode<T>> &node) {
        if (node->get_type() == TYPE::LEAF) {
            auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(node);
            leaf->remove_element(element);
        }
    });
    return true;
}

template<typename T, int arr_size>
T Tree<T, arr_size>::get_by_index(size_t index) {
    return get_by_index_helper(root, index);
}

template<typename T, int arr_size>
T Tree<T, arr_size>::operator[](const int index) {
    return get_by_index(index);
}

template<typename T, int arr_size>
bool Tree<T, arr_size>::sort() {
    if (!root) {
        return false;
    }

    std::vector<T> elements = get_all_elements();
    std::sort(elements.begin(), elements.end());

    const size_t leaf_count = count_leaf_nodes(root);

    if (leaf_count == 0) {
        throw std::runtime_error("No leaf nodes found in the tree.");
    }

    size_t total_elements = elements.size();
    size_t elements_per_leaf = total_elements / leaf_count;

    auto it = elements.begin();

    clear_with_struct();
    distribute_elements(root, it, elements_per_leaf + 1, total_elements);

    isTreeSorted = true;
    return true;
}

/**
 * @brief Рекурсивная функция для подсчета количества конечных узлов
 * @param node указатель на вершину дерева
 * @return 0 - если не конечная вершина
 * @return 1 - если конечная вершина
 */
template<typename T, int arr_size>
size_t Tree<T, arr_size>::count_leaf_nodes(const std::shared_ptr<TreeNode<T>> &node) const {
    if (!node) {
        return 0;
    }

    if (node->get_type() == TYPE::LEAF) {
        return 1;
    }

    auto intermediate = std::dynamic_pointer_cast<IntermediateNode<T, arr_size> >(node);
    return count_leaf_nodes(intermediate->get_left_node()) +
           count_leaf_nodes(intermediate->get_right_node());
}

/**
 * @brief Функция для получения всех элементов дерева
 * @return Массив элементов
 */
template<typename T, int arr_size>
std::vector<T> Tree<T, arr_size>::get_all_elements() {
    std::vector<T> elements;
    traverse(root, [&](const std::shared_ptr<TreeNode<T>> &node) {
        if (node->get_type() == TYPE::LEAF) {
            auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(node);
            leaf->get_all_elements(elements);
        }
    });
    return elements;
}

/**
 * Функция для вывода значений дерева при проходе 'в ширину'
 */
template<typename T, int arr_size>
void Tree<T, arr_size>::in_order_traversal(bool is_need_to_print) {
    traverse(root, [&](const std::shared_ptr<TreeNode<T>> &node) {
        if (node->get_type() == TYPE::LEAF) {
            auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(node);
            std::vector<T> elements;
            leaf->get_all_elements(elements);
            if (is_need_to_print) {
                for (auto &element: elements) {
                    std::cout << element << " ";
                }
            }
        }
    });
}

/**
 * @brief Функция для сохранения дерева в бинарный файл
 * @param ofs Поток ввода
 */
template<typename T, int arr_size>
void Tree<T, arr_size>::save_to_binary_file(std::ofstream &ofs) {
    if (!root) {
        uint8_t tree_status = 0; // дерево пустое
        ofs.write(reinterpret_cast<const char *>(&tree_status), sizeof(tree_status));
        return;
    }

    uint8_t tree_status = 1; // дерево существует
    ofs.write(reinterpret_cast<const char *>(&tree_status), sizeof(tree_status));

    auto save_node = [&ofs](const std::shared_ptr<TreeNode<T>> &node) {
        if (node->get_type() == TYPE::LEAF) {
            auto type = static_cast<uint8_t>(TYPE::LEAF);
            ofs.write(reinterpret_cast<const char *>(&type), sizeof(type));
            auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(node);
            const size_t size = leaf->get_size();
            ofs.write(reinterpret_cast<const char *>(&size), sizeof(size));
            std::vector<T> elements;
            leaf->get_all_elements(elements);
            for (const auto &element : elements) {
                ofs.write(reinterpret_cast<const char *>(&element), sizeof(T));
            }
        } else if (node->get_type() == TYPE::INTERMEDIATE) {
            const auto type = static_cast<uint8_t>(TYPE::INTERMEDIATE);
            ofs.write(reinterpret_cast<const char *>(&type), sizeof(type));
        }
    };

    traverse(root, save_node);
}

/**
 * @brief Функция для загрузки дерева из бинарного файла
 * @param ifs Поток выходных данных
 */
template<typename T, int arr_size>
void Tree<T, arr_size>::load_from_binary_file(std::ifstream &ifs) {
    if (!ifs.is_open()) {
        throw std::runtime_error("Ошибка: файл не удалось открыть для чтения.");
    }

    uint8_t tree_status;
    ifs.read(reinterpret_cast<char *>(&tree_status), sizeof(tree_status));
    if (!ifs || tree_status == 0) {
        root.reset();
        return; // пустое дерево
    }

    std::function<std::shared_ptr<TreeNode<T>>()> load_node = [&]() {
        uint8_t type;
        ifs.read(reinterpret_cast<char *>(&type), sizeof(type));

        if (static_cast<TYPE>(type) == TYPE::LEAF) {
            size_t size;
            ifs.read(reinterpret_cast<char *>(&size), sizeof(size));

            auto leaf = std::make_shared<LeafNode<T, arr_size>>();
            for (size_t i = 0; i < size; ++i) {
                T element;
                ifs.read(reinterpret_cast<char *>(&element), sizeof(T));
                leaf->add_element(element);
            }
            return std::static_pointer_cast<TreeNode<T>>(leaf);
        }

        if (static_cast<TYPE>(type) == TYPE::INTERMEDIATE) {
            auto intermediate = std::make_shared<IntermediateNode<T, arr_size>>();
            intermediate->set_left_node(load_node());
            intermediate->set_right_node(load_node());
            return std::static_pointer_cast<TreeNode<T>>(intermediate);
        }

        return std::shared_ptr<TreeNode<T>>(nullptr);
    };

    root = load_node();
}

/**
 * @brief Функция для вывода дерева в понятном для восприятия формате
 */
template<typename T, int arr_size>
void Tree<T, arr_size>::print_helper() {
    if (!root) {
        std::cout << "Empty Tree" << std::endl;
        return;
    }

    std::queue<std::shared_ptr<TreeNode<T>> > queue;
    queue.push(root);

    while (!queue.empty()) {
        const size_t level_size = queue.size();

        for (size_t i = 0; i < level_size; ++i) {
            auto current = queue.front();
            queue.pop();

            if (current->get_type() == TYPE::LEAF) {
                auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(current);
                std::cout << "[";
                for (size_t j = 0; j < leaf->get_size(); ++j) {
                    std::cout << leaf->get_element_at(j);
                    if (j + 1 < leaf->get_size()) std::cout << ", ";
                }
                std::cout << "] ";
            } else if (current->get_type() == TYPE::INTERMEDIATE) {
                std::cout << "INT ";
                auto intermediate = std::dynamic_pointer_cast<IntermediateNode<T, arr_size> >(current);
                queue.push(intermediate->get_left_node());
                queue.push(intermediate->get_right_node());
            }
        }
        std::cout << "\n";
    }
}

template<typename T, int arr_size>
bool Tree<T, arr_size>::insert_by_index(size_t index, const T &element) {
    return insert_helper(root, index, element);
}

template<typename T, int arr_size>
bool Tree<T, arr_size>::remove_by_index(const size_t index) {
    return remove_helper(root, index);
}

template<typename T, int arr_size>
bool Tree<T, arr_size>::insert_with_order_save(T element) {
    if (!root) {
        auto new_leaf = std::make_shared<LeafNode<T, arr_size>>();
        new_leaf->add_element(element);
        root = new_leaf;
        return true;
    }
    if (!isTreeSorted) {
        std::cout << "Cannot insert value with order save in not sorted tree\n";
        return false;
    }

    return insert_with_order_helper(root, element);
}

template<typename T, int arr_size>
void Tree<T, arr_size>::clear_with_struct() {
    traverse(root, [](const std::shared_ptr<TreeNode<T>> &node) {
        if (node->get_type() == TYPE::LEAF) {
            auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(node);
            leaf->clear_elements();
        }
    });
}

template<typename T, int arr_size>
void Tree<T, arr_size>::distribute_elements(const std::shared_ptr<TreeNode<T>> &node,
                                            typename std::vector<T>::iterator &it,
                                            const size_t elements_per_leaf,
                                            size_t &remaining_elements) {
    if (!node) {
        return;
    }

    if (node->get_type() == TYPE::LEAF) {
        auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size>>(node);
        if (!leaf) {
            throw std::runtime_error("Failed to cast to LeafNode");
        }

        size_t added_elements = 0;
        while (added_elements < elements_per_leaf && remaining_elements > 0) {
            leaf->add_element(*it);
            ++it;
            ++added_elements;
            --remaining_elements;
        }

        return;
    }

    auto intermediate = std::dynamic_pointer_cast<IntermediateNode<T, arr_size>>(node);
    if (!intermediate) {
        throw std::runtime_error("Failed to cast to IntermediateNode");
    }

    distribute_elements(intermediate->get_left_node(), it, elements_per_leaf, remaining_elements);
    distribute_elements(intermediate->get_right_node(), it, elements_per_leaf, remaining_elements);
}

template<typename T, int arr_size>
T Tree<T, arr_size>::get_by_index_helper(std::shared_ptr<TreeNode<T>> node, size_t index) {
    if (!node) {
        throw std::out_of_range("Index out of bounds");
    }

    if (node->get_type() == TYPE::LEAF) {
        if (auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size>>(node); index < leaf->get_size()) {
            return leaf->get_element_at(index);
        } else {
            throw std::out_of_range("Index out of bounds");
        }
    }

    if (node->get_type() == TYPE::INTERMEDIATE) {
        auto intermediate = std::dynamic_pointer_cast<IntermediateNode<T, arr_size>>(node);

        if (const size_t left_size = intermediate->get_left_node()->get_size(); index < left_size) {
            return get_by_index_helper(intermediate->get_left_node(), index);
        } else {
            return get_by_index_helper(intermediate->get_right_node(), index - left_size);
        }
    }

    throw std::runtime_error("Unexpected node type");
}

template<typename T, int arr_size>
bool Tree<T, arr_size>::insert_with_order_helper(std::shared_ptr<TreeNode<T>> &node, const T &element) {
    if (node->get_type() == TYPE::LEAF) {
        auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size>>(node);

        size_t pos = 0;
        while (pos < leaf->get_size() && leaf->get_element_at(pos) < element) {
            ++pos;
        }

        if (!leaf->insert_by_index(pos, element)) {
            const size_t mid = arr_size / 2;

            auto new_left_node = std::make_shared<LeafNode<T, arr_size>>();
            auto new_right_node = std::make_shared<LeafNode<T, arr_size>>();

            for (size_t i = 0; i < mid; ++i) {
                new_left_node->add_element(leaf->get_element_at(i));
            }
            for (size_t i = mid; i < leaf->get_size(); ++i) {
                new_right_node->add_element(leaf->get_element_at(i));
            }

            if (pos < mid) {
                new_left_node->insert_by_index(pos, element);
            } else {
                new_right_node->insert_by_index(pos - mid, element);
            }

            auto intermediate = std::make_shared<IntermediateNode<T, arr_size>>();
            intermediate->set_left_node(new_left_node);
            intermediate->set_right_node(new_right_node);

            node = intermediate;
        }
        return true;
    }

    if (node->get_type() == TYPE::INTERMEDIATE) {
        auto intermediate = std::dynamic_pointer_cast<IntermediateNode<T, arr_size>>(node);

        const size_t left_size = intermediate->get_left_node() ? intermediate->get_left_node()->get_size() : 0;

        if (element <= intermediate->get_left_node()->get_max_value()) {
            return insert_with_order_helper(intermediate->get_left_node(), element);
        } else {
            return insert_with_order_helper(intermediate->get_right_node(), element);
        }
    }

    return false;
}
