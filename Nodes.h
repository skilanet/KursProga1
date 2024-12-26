#pragma once
#include <sstream>

/**
 * Classes
 *
 * TYPE - класс для типа вершины
 *
 * TreeNode<T> - абстрактный класс вершины
 *
 * LeafNode - класс конечной вершины
 *
 * IntermediateNode - класс промежуточной вершины
 */
enum class TYPE { LEAF, INTERMEDIATE };


/**
 * Класс TreeNode<T> является интерфейсом, он позволяет использовать полиморфизм для лучшего управления поддеревьями
 */
template<typename T>
class TreeNode {
public:
    virtual ~TreeNode() = default;

    virtual TYPE get_type() = 0;

    virtual std::string to_string() = 0;

    virtual size_t get_size() = 0;

    virtual explicit operator std::string() = 0;

    virtual T get_max_value() = 0;
};

/**
 * @brief Класс итоговой вершины дерева
 * @tparam T используется для задания типа данных массива
 * @tparam arr_size используется для задания размера массива на этапе компиляции
 *
 * при инициализации создаётся умный указатель на массив с данными и последний элемент становиться NULL
 */
template<typename T, size_t arr_size>
class LeafNode final : public TreeNode<T> {
    std::unique_ptr<T[]> data;
    size_t actual_size = 0;

public:
    void get_all_elements(std::vector<T> &elements);

    TYPE get_type() override { return TYPE::LEAF; }

    LeafNode() {
        data = std::make_unique<T[]>(arr_size + 1);
        data[actual_size] = NULL;
    }

    size_t get_size() override { return actual_size; }

    std::string to_string() override;

    bool add_element(T element);

    bool remove_element(T element);

    bool remove_by_index(int index);

    explicit operator std::string() override { return to_string(); }

    T get_element_at(size_t index) const {
        if (index >= actual_size) {
            throw std::out_of_range("index out of range");
        }
        return data[index];
    }

    void add_elements(std::vector<T> elements) {
        for (auto element : elements) {
            add_element(element);
        }
    }

    bool insert_by_index(size_t index, T element);

    bool remove_by_index(size_t index);

    bool clear_elements();

    std::vector<T> get_data() {
        std::vector<T> elements;
        get_all_elements(elements);
        return elements;
    }

    T get_max_value() override { return data[actual_size - 1]; }
};

/**
 * @brief Класс для промежуточных вершин
 * @tparam T используется для задания типа данных массива (нужна для корректного создания LeafNode)
 * @tparam arr_size используется для задания размера массива на этапе компиляции (нужна для корректного создания LeafNode)
 *
 * @param left_node указатель на левое поддерево, используется умный указатель, а так же тип TreeNode<T>
 * @param right_node указатель на правое поддерево, используется умный указатель, а так же тип TreeNode<T>
 *
 * при инициализации указатели на поддеревья по-умолчанию имеют тип nullptr
 */
template<typename T, size_t arr_size>
class IntermediateNode final : public TreeNode<T> {
    std::shared_ptr<TreeNode<T>> left_node;
    std::shared_ptr<TreeNode<T>> right_node;

public:
    void get_all_elements(std::vector<T> &elements);

    TYPE get_type() override { return TYPE::INTERMEDIATE; }

    explicit IntermediateNode(): left_node(nullptr), right_node(nullptr) {
    }

    std::string to_string() override;

    size_t get_size() override {
        const size_t left_size = left_node ? left_node->get_size() : 0;
        const size_t right_size = right_node ? right_node->get_size() : 0;
        return left_size + right_size;
    }

    explicit operator std::string() override { return to_string(); }

    std::shared_ptr<TreeNode<T>> &get_left_node();

    std::shared_ptr<TreeNode<T>> &get_right_node();

    bool set_left_node(const std::shared_ptr<TreeNode<T>> &new_left_node);

    bool set_right_node(const std::shared_ptr<TreeNode<T>> &new_right_node);

    T get_max_value() override {
        if (left_node && right_node) {
            return std::max(left_node->get_max_value(), right_node->get_max_value());
        }
    };
};

/**
 * @brief функция для создания строки из объекта
 */
template<typename T, size_t arr_size>
std::string IntermediateNode<T, arr_size>::to_string() {
    std::ostringstream os;
    os << "IntermediateNode left - (";
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
    os << ")\n";
    return os.str();
}


/**
 * @brief Функция рекурсивно получает все элементы в каждой ветви
 * @param elements Указатель на вектор элементов
 */
template<typename T, size_t arr_size>
void IntermediateNode<T, arr_size>::get_all_elements(std::vector<T> &elements) {
    if (left_node || left_node->get_type() == TYPE::INTERMEDIATE) {
        if (left_node->get_type() == TYPE::INTERMEDIATE) {
            std::dynamic_pointer_cast<IntermediateNode>(left_node)->get_all_elements(elements);
        } else {
            std::dynamic_pointer_cast<LeafNode<T, arr_size> >(left_node)->get_all_elements(elements);
        }
    }
    if (right_node || right_node->get_type() == TYPE::INTERMEDIATE) {
        if (right_node->get_type() == TYPE::INTERMEDIATE) {
            std::dynamic_pointer_cast<IntermediateNode>(right_node)->get_all_elements(elements);
        } else {
            std::dynamic_pointer_cast<LeafNode<T, arr_size> >(right_node)->get_all_elements(elements);
        }
    }
}

/**
 * @brief Getter для левого поддерева
 * @return Функция возвращает указатель на левое поддерево
 */
template<typename T, size_t arr_size>
std::shared_ptr<TreeNode<T>> &IntermediateNode<T, arr_size>::get_left_node() {
    return left_node;
}

/**
 * @brief Getter для правого поддерева
 * @return Функция возвращает указатель на правое поддерево
 */
template<typename T, size_t arr_size>
std::shared_ptr<TreeNode<T>> &IntermediateNode<T, arr_size>::get_right_node() {
    return right_node;
}

/**
 * @brief Setter для левого поддерева
 * @param new_left_node Указатель на новое поддерево
 * @return true - если смена произошла
 * @return false - если возникли ошибки
 */
template<typename T, size_t arr_size>
bool IntermediateNode<T, arr_size>::set_left_node(const std::shared_ptr<TreeNode<T>> &new_left_node) {
    left_node = std::dynamic_pointer_cast<TreeNode<T>>(new_left_node);
    return left_node != nullptr && left_node->get_type() == TYPE::INTERMEDIATE;
}

/**
 * @brief Setter для правого поддерева
 * @param new_right_node Указатель на новое поддерево
 * @return true - если смена произошла
 * @return false - если возникли ошибки
 */
template<typename T, size_t arr_size>
bool IntermediateNode<T, arr_size>::set_right_node(const std::shared_ptr<TreeNode<T>> &new_right_node) {
    right_node = std::dynamic_pointer_cast<TreeNode<T>>(new_right_node);
    return right_node != nullptr && right_node->get_type() == TYPE::INTERMEDIATE;
}

/**
 * @brief Функция превращает промежуточный узел в подстроку
 * @return Строку состоящую из преобразованного конечного узла
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


/**
 * @brief Функция добавляет элемент в список элементов
 * @param element Новый элемент
 * @return true - Если элемент добавился
 * @return false - Если при добавлении произошла ошибка
 */
template<typename T, size_t arr_size>
bool LeafNode<T, arr_size>::add_element(T element) {
    if (actual_size < arr_size) {
        if constexpr (std::is_same_v<T, char *>) {
            size_t len = std::strlen(element);
            std::cout << len << std::endl;
            data[actual_size] = new char[len + 1];
            std::strcpy(data[actual_size], element);
            actual_size++;
            return true;
        } else {
            data[actual_size++] = element;
            data[actual_size] = NULL;
            return true;
        }
    }
    return false;
}

/**
 * @brief Функция удаляет элемент
 * @param element Элемент который необходимо удалить
 * @return true - Если удаление произошло успешно
 * @return false - Если возникли проблемы при удалении 
 */
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

/**
 * @brief Функция для удаления элемента по индексу
 * @param index Индекс на котором стоит элемент, который необходимо удалить
 * @return 
 */
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

template<typename T, size_t arr_size>
bool LeafNode<T, arr_size>::insert_by_index(size_t index, T element) {
    if (index > arr_size || actual_size >= arr_size) {
        return false;
    }

    for (size_t i = actual_size; i > index; --i) {
        data[i] = data[i - 1];
    }

    data[index] = element;

    ++actual_size;
    return true;
}

template<typename T, size_t arr_size>
bool LeafNode<T, arr_size>::remove_by_index(const size_t index) {
    if (index > arr_size || actual_size >= arr_size) {
        throw std::out_of_range("Index out of bounds or leaf is full");
    }

    for (size_t i = index; i < actual_size - 1; ++i) {
        data[i] = data[i + 1];
    }
    --actual_size;
    return true;
}

template<typename T, size_t arr_size>
bool LeafNode<T, arr_size>::clear_elements() {
    data = std::make_unique<T[]>(arr_size + 1);
    actual_size = 0;
    data[actual_size] = NULL;
    return true;
}

/**
 * @brief Получает все элементы в конечном узле
 * @param elements Указатель на вектор элементор
 */
template<typename T, size_t arr_size>
void LeafNode<T, arr_size>::get_all_elements(std::vector<T> &elements) {
    for (int i = 0; data[i] != NULL; i++) {
        elements.push_back(data[i]);
    }
}
