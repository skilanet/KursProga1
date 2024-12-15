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
    std::shared_ptr<TreeNode> root;

    std::vector<std::shared_ptr<LeafNode<T, arr_size> > > nodes;

    void traverse(const std::shared_ptr<TreeNode> &root, const std::function<void(std::shared_ptr<TreeNode>)> &func);

    bool insert_helper(std::shared_ptr<TreeNode> &node, const T &element);

    std::shared_ptr<TreeNode> build_balanced_tree(const std::vector<T> &elements, int start, int end);

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
        tree.traverse(tree.root, [&](const std::shared_ptr<TreeNode> &node) {
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

    void balance_intermediate(std::shared_ptr<IntermediateNode<T, arr_size> > &node);

public:
    Tree(): root(nullptr) {
    }

    void in_order_traversal();

    ~Tree() = default;

    bool insert(const T &element);

    bool remove(const T &element);

    bool remove_by_index(int index);

    T get_by_index(size_t index);

    T operator[](int index);

    bool sort();

    void save_to_binary_file(std::ofstream &ofs);

    void load_from_binary_file(std::ifstream &ifs);

    std::string to_string();

    void print_helper();

    void print_tree() {
        print_helper();
    }

    void clear() { root = nullptr; }
};

/**
 * @brief Базовая функция для работы с деревом
 * @param root Указатель на вершину дерева
 * @param func Указатель на функцию для того, чтобы её можно было переиспользовать
 */
template<typename T, int arr_size>
void Tree<T, arr_size>::traverse(const std::shared_ptr<TreeNode> &root,
                                 const std::function<void(std::shared_ptr<TreeNode>)> &func) {
    if (root == nullptr) return;
    const auto current = std::dynamic_pointer_cast<TreeNode>(root);
    func(current);
    if (current->get_type() == TYPE::INTERMEDIATE) {
        auto intermediate = std::dynamic_pointer_cast<IntermediateNode<T, arr_size> >(current);
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
bool Tree<T, arr_size>::insert_helper(std::shared_ptr<TreeNode> &node, const T &element) {
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
        auto new_intermediate = std::make_shared<IntermediateNode<T, arr_size> >();
        auto left_leaf = std::make_shared<LeafNode<T, arr_size> >();
        auto right_leaf = std::make_shared<LeafNode<T, arr_size> >();
        std::vector<T> elements;
        leaf->get_all_elements(elements);
        for (size_t i = 0; i < elements.size(); ++i) {
            if (i < elements.size() / 2) {
                left_leaf->add_element(elements[i]);
            } else {
                right_leaf->add_element(elements[i]);
            }
        }
        right_leaf->add_element(element);
        new_intermediate->set_left_node(left_leaf);
        new_intermediate->set_right_node(right_leaf);
        node = new_intermediate;
        return true;
    }

    if (node->get_type() == TYPE::INTERMEDIATE) {
        auto intermediate = std::dynamic_pointer_cast<IntermediateNode<T, arr_size> >(node);
        auto left_size = intermediate->get_left_node()->get_size();
        auto right_size = intermediate->get_right_node()->get_size();

        if (left_size <= right_size) {
            insert_helper(intermediate->get_left_node(), element);
        } else {
            insert_helper(intermediate->get_right_node(), element);
        }
        left_size = intermediate->get_left_node()->get_size();
        right_size = intermediate->get_right_node()->get_size();

        if (std::__math::fabs(left_size - right_size) > 1) {
            balance_intermediate(intermediate);
        }

        return true;
    }
    return false;
}

/**
 * @brief Функция которая используется для вызова функции bool Tree<T, arr_size>::insert_helper(std::shared_ptr<TreeNode> &node, const T &element)
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
    traverse(root, [&](const std::shared_ptr<TreeNode> &node) {
        if (node->get_type() == TYPE::LEAF) {
            auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(node);
            leaf->remove_element(element);
        }
    });
    return true;
}

template<typename T, int arr_size>
bool Tree<T, arr_size>::remove_by_index(const int index) {
    std::vector<T> elements;
    traverse(root, [&](const std::shared_ptr<TreeNode> &node) {
        if (node->get_type() == TYPE::LEAF) {
            auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(node);
            leaf->get_all_elements(elements);
        }
    });
    std::vector<T> new_elements;
    for (size_t i = 0; i < elements.size(); ++i) {
        if (i != index) {
            new_elements.push_back(elements[i]);
        }
    }
    root = nullptr;
    for (auto &element: new_elements) {
        this->insert(element);
    }
    return true;
}

template<typename T, int arr_size>
T Tree<T, arr_size>::get_by_index(size_t index) {
    std::vector<T> elements;
    traverse(root, [&](const std::shared_ptr<TreeNode> &node) {
        if (node->get_type() == TYPE::LEAF) {
            auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(node);
            leaf->get_all_elements(elements);
        }
    });
    if (index >= elements.size()) {
        throw std::out_of_range("index out of range");
    }
    return elements[index];
}

template<typename T, int arr_size>
T Tree<T, arr_size>::operator[](const int index) {
    return get_by_index(index);
}

template<typename T, int arr_size>
bool Tree<T, arr_size>::sort() {
    if (!root) {
        return true;
    }

    std::vector<T> elements = get_all_elements();

    std::sort(elements.begin(), elements.end());
    root = nullptr;
    for (auto &element: elements) {
        insert(element);
    }
    return true;
}

/**
 * @brief Функция для получения всех элементов дерева
 * @return Массив элементов
 */
template<typename T, int arr_size>
std::vector<T> Tree<T, arr_size>::get_all_elements() {
    std::vector<T> elements;
    traverse(root, [&](const std::shared_ptr<TreeNode> &node) {
        if (node->get_type() == TYPE::LEAF) {
            auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(node);
            leaf->get_all_elements(elements);
        }
    });
    return elements;
}

/**
 * @brief Функция для балансировки элементов
 * @param node Указатель на вершину дерева
 */
template<typename T, int arr_size>
void Tree<T, arr_size>::balance_intermediate(std::shared_ptr<IntermediateNode<T, arr_size> > &node) {
    auto left = node->get_left_node();
    auto right = node->get_right_node();
    std::vector<T> all_elements;
    if (left->get_type() == TYPE::LEAF) {
        auto left_leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(left);
        left_leaf->get_all_elements(all_elements);
    } else {
        auto left_intermediate = std::dynamic_pointer_cast<IntermediateNode<T, arr_size> >(left);
        left_intermediate->get_all_elements(all_elements);
    }
    if (right->get_type() == TYPE::LEAF) {
        auto right_leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(right);
        right_leaf->get_all_elements(all_elements);
    } else {
        auto right_intermediate = std::dynamic_pointer_cast<IntermediateNode<T, arr_size> >(right);
        right_intermediate->get_all_elements(all_elements);
    }
    std::sort(all_elements.begin(), all_elements.end());
    const size_t mid = all_elements.size() / 2;
    auto new_left_leaf = std::make_shared<LeafNode<T, arr_size> >();
    for (size_t i = 0; i < mid; ++i) {
        new_left_leaf->add_element(all_elements[i]);
    }
    auto new_right_leaf = std::make_shared<LeafNode<T, arr_size> >();
    for (size_t i = mid; i < all_elements.size(); ++i) {
        new_right_leaf->add_element(all_elements[i]);
    }
    node->set_left_node(new_left_leaf);
    node->set_right_node(new_right_leaf);
}

/**
 * Функция для вывода значений дерева при проходе 'в ширину'
 */
template<typename T, int arr_size>
void Tree<T, arr_size>::in_order_traversal() {
    traverse(root, [&](const std::shared_ptr<TreeNode> &node) {
        if (node->get_type() == TYPE::LEAF) {
            auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(node);
            std::vector<T> elements;
            leaf->get_all_elements(elements);
            for (auto &element: elements) {
                std::cout << element << " ";
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
    if (!root) return;

    auto save_node = [&ofs](const std::shared_ptr<TreeNode> &node) {
        if (node->get_type() == TYPE::LEAF) {
            auto type = static_cast<uint8_t>(TYPE::LEAF);
            ofs.write(reinterpret_cast<const char *>(&type), sizeof(type));
            auto leaf = std::dynamic_pointer_cast<LeafNode<T, arr_size> >(node);
            const size_t size = leaf->get_size();
            ofs.write(reinterpret_cast<const char *>(&size), sizeof(size));
            std::vector<T> elements;
            leaf->get_all_elements(elements);
            for (const auto &element: elements) {
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
    root.reset();

    std::function<std::shared_ptr<TreeNode>()> load_node = [&]() {
        uint8_t type;
        ifs.read(reinterpret_cast<char *>(&type), sizeof(type));

        if (static_cast<TYPE>(type) == TYPE::LEAF) {
            size_t size;
            ifs.read(reinterpret_cast<char *>(&size), sizeof(size));

            auto leaf = std::make_shared<LeafNode<T, arr_size> >();
            for (size_t i = 0; i < size; ++i) {
                T element;
                ifs.read(reinterpret_cast<char *>(&element), sizeof(T));
                leaf->add_element(element);
            }
            return std::static_pointer_cast<TreeNode>(leaf);
        }
        if (static_cast<TYPE>(type) == TYPE::INTERMEDIATE) {
            auto intermediate = std::make_shared<IntermediateNode<T, arr_size> >();
            intermediate->set_left_node(load_node());
            intermediate->set_right_node(load_node());
            return std::static_pointer_cast<TreeNode>(intermediate);
        }

        return std::shared_ptr<TreeNode>(nullptr);
    };

    root = load_node();
}

/**
 * @brief Функция для вывода дерева в понятном для восприятия формате
 */
template<typename T, int arr_size>
void Tree<T, arr_size>::print_helper() {
    if (!root) return;

    std::queue<std::shared_ptr<TreeNode> > queue;
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
