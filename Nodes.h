#pragma once
#include <iostream>
#include <sstream>

/**
 * Classes
 *
 * TYPE - класс для типа вершины
 *
 * TreeNode - абстрактный класс вершины
 *
 * LeafNode - класс конечной вершины
 *
 * IntermediateNode - класс промежуточной вершины
 */
enum class TYPE { LEAF, INTERMEDIATE };


/**
 * Класс TreeNode является интерфейсом, он позволяет использовать полиморфизм для лучшего управления поддеревьями
 */
class TreeNode {
public:
    virtual ~TreeNode() = default;

    virtual TYPE get_type() = 0;

    virtual bool change_type() = 0;

    virtual std::string to_string() = 0;

    virtual explicit operator std::string() = 0;

    size_t depth{};

    size_t index{};

};

/**
 * @brief Класс итоговой вершины дерева
 * @tparam T используется для задания типа данных массива
 * @tparam arr_size используется для задания размера массива на этапе компиляции
 *
 * при инициализации создаётся умный указатель на массив с данными и последний элемент становиться NULL
 */
template<typename T, size_t arr_size>
class LeafNode final : public TreeNode {
    std::unique_ptr<T[]> data;
    size_t actual_size = 0;
    size_t depth;
    size_t index;
public:
    TYPE get_type() override { return TYPE::LEAF; }

    LeafNode(const size_t depth, const size_t index) : depth(depth), index(index) {
        data = std::make_unique<T[]>(arr_size + 1);
        data[actual_size] = NULL;
    };

    bool change_type() override { return true; };

    std::string to_string() override;

    bool add_element(T element);

    bool remove_element(T element);

    bool remove_by_index(int index);

    explicit operator std::string() override { return to_string(); }
};

/**
 * @brief Класс для промежуточных вершин
 * @tparam T используется для задания типа данных массива (нужна для корректного создания LeafNode)
 * @tparam arr_size используется для задания размера массива на этапе компиляции (нужна для корректного создания LeafNode)
 *
 * @param left_node указатель на левое поддерево, используется умный указатель, а так же тип TreeNode
 * @param right_node указатель на правое поддерево, используется умный указатель, а так же тип TreeNode
 *
 * при инициализации указатели на поддеревья по-умолчанию имеют тип nullptr
 */
template<typename T, size_t arr_size>
class IntermediateNode final : public TreeNode {
    std::shared_ptr<TreeNode> left_node;
    std::shared_ptr<TreeNode> right_node;

public:
    TYPE get_type() override { return TYPE::INTERMEDIATE; }

    std::shared_ptr<TreeNode> get_left_node();

    std::shared_ptr<TreeNode> get_right_node();

    bool create_node();

    bool change_type() override;

    IntermediateNode(): left_node(nullptr), right_node(nullptr) {
    }

    ~IntermediateNode() override = default;

    std::string to_string() override;

    explicit operator std::string() override { return to_string(); }
};

/**
 * @brief Функция для инициализации поддеревьев если значение поддерева == nullptr создаётся новая промежуточная вершина
 * @return true - если поддерево создано успешно
 * @return false - если по какой-либо причине поддерево создать не удалось
 */
template<typename T, size_t arr_size>
bool IntermediateNode<T, arr_size>::create_node() {
    if (left_node == nullptr) {
        left_node = std::make_shared<IntermediateNode>();
        return left_node != nullptr;
    }
    right_node = std::make_shared<IntermediateNode>();
    return right_node != nullptr;
}

/**
 * @brief Функция для смены типа поддерева
 * при вызове функции если поддерево не nullptr и имеет тип промежуточной вершины то создаётся новый указатель на объект
 * типа LeafNode и заменяет предыдущий указатель
 * @return true - если тип сменился успешно
 * @return false - если что-то пошло не так при смене типа
 */
template<typename T, size_t arr_size>
bool IntermediateNode<T, arr_size>::change_type()  {
    if (left_node && left_node->get_type() == TYPE::INTERMEDIATE) {
        left_node.reset();
        left_node = std::make_shared<LeafNode<T, arr_size> >();
        return left_node != nullptr && left_node->get_type() == TYPE::LEAF;
    }
    if (right_node && right_node->get_type() == TYPE::INTERMEDIATE) {
        left_node.reset();
        right_node = std::make_shared<LeafNode<T, arr_size> >();
        return right_node != nullptr && right_node->get_type() == TYPE::LEAF;
    }
    return false;
}

/**
 * @brief функция для создания строки из объекта
 */
template<typename T, size_t arr_size>
std::string IntermediateNode<T, arr_size>::to_string() {
    std::ostringstream os;
    os << "IntermediateNode: left - (";
    if (left_node) {
        os << left_node.get();
    } else {
        os << "nullptr";
    }
    os << "); right - ";
    if (right_node) {
        os << right_node.get();
    } else {
        os << "nullptr";
    }
    return os.str();
}

template<typename T, size_t arr_size>
std::shared_ptr<TreeNode> IntermediateNode<T, arr_size>::get_left_node() {
    return left_node;
}

template<typename T, size_t arr_size>
std::shared_ptr<TreeNode> IntermediateNode<T, arr_size>::get_right_node() {
    return right_node;
}


/**
 * Методы класса LeafNode
 *
 * в одном файле из-за наличия template<class T, size_t arr_size>
 */
template<typename T, size_t arr_size>
std::string LeafNode<T, arr_size>::to_string() {
    std::ostringstream os;
    os << "LeafNode(actual_size = " << actual_size << ")" << ": [";
    for (int i = 0; data[i] != NULL; i++) {
        if (data[i + 1] == NULL) {
            os << data[i];
        } else {
            os << data[i] << ", ";
        }
    }
    os << "]" << std::endl;
    return os.str();
}

template<typename T, size_t arr_size>
bool LeafNode<T, arr_size>::add_element(T element) {
    if (actual_size < arr_size) {
        data[actual_size++] = element;
        data[actual_size] = NULL;
        return true;
    }
    return false;
}

template<typename T, size_t arr_size>
bool LeafNode<T, arr_size>::remove_element(T element) {
    std::unique_ptr<T[]> temp = std::make_unique<T[]>(arr_size);
    for (int i = 0, j = 0; data[i] != NULL; i++) {
        if (data[i] != element) {
            temp[j] = data[i];
            j++;
        } else {
            actual_size--;
        }
    }
    data = std::move(temp);
    return true;
}

template<typename T, size_t arr_size>
bool LeafNode<T, arr_size>::remove_by_index(const int index) {
    if (index < 0 || index >= actual_size) {
        throw std::out_of_range("Leaf node index out of range");
    }
    std::unique_ptr<T[]> temp = std::make_unique<T[]>(arr_size);
    for (int i = 0, j = 0; data[i] != NULL; i++) {
        if (i != index) {
            temp[j] = data[i];
            j++;
        } else {
            actual_size--;
        }
    }
    data = std::move(temp);
    return true;
}

