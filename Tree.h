#pragma once
#include <iostream>

#include "Nodes.h"

template<typename T, int arr_size>
class Tree final {
    std::shared_ptr<TreeNode> root;

    std::vector<std::shared_ptr<LeafNode<T, arr_size> > > nodes;

    void traverse(const std::shared_ptr<TreeNode> &root, const std::function<void(std::shared_ptr<TreeNode>)> &func);

    friend std::ostream &operator<<(std::ostream &os, const std::shared_ptr<TreeNode> &root);

    friend std::istream &operator>>(std::istream &is, std::shared_ptr<TreeNode> &root);

    void add_to_nodes(const std::shared_ptr<TreeNode> &node);

public:
    Tree(): root(nullptr) {}

    ~Tree() = default;

    bool insert(const T &element);

    bool remove(const T &element);

    T get_by_index(int index) const;

    T operator[](int index) const;

    bool sort();

    bool balance();

    void save_to_binary_file(const std::ofstream &ofs) const;

    void load_from_binary_file(std::ifstream &ifs);

    std::string to_string();
};

template<typename T, int arr_size>
void Tree<T, arr_size>::traverse(const std::shared_ptr<TreeNode> &root,
                                 const std::function<void(std::shared_ptr<TreeNode>)> &func) {
    if (root == nullptr) return;
    const auto current = std::dynamic_pointer_cast<TreeNode>(root);
    func(current);
    if (current->get_type() == TYPE::INTERMEDIATE) {
        traverse(std::dynamic_pointer_cast<IntermediateNode<T, arr_size> >(current)->get_left_node(), func);
        traverse(std::dynamic_pointer_cast<IntermediateNode<T, arr_size> >(current)->get_right_node(), func);
    }
}

template<typename T, int arr_size>
void Tree<T, arr_size>::add_to_nodes(const std::shared_ptr<TreeNode> &node) {
    if (node->get_type() == TYPE::LEAF) {
        nodes.push_back(std::dynamic_pointer_cast<LeafNode<T, arr_size> >(node));
    }
}

template<typename T, int arr_size>
bool Tree<T, arr_size>::insert(const T &element) {
    if (!root) root = std::make_shared<LeafNode<T, arr_size> >();
    traverse(root, [this](const std::shared_ptr<TreeNode> &node) {
        if (node->get_type() == TYPE::LEAF) {
            nodes.push_back(std::dynamic_pointer_cast<LeafNode<T, arr_size>>(node));
        }
    });
    for (const auto node: nodes) {
        if (node->get_type() == TYPE::LEAF) {
            node->add_element(element);
        }
        return true;
    }
    return false;
}

template<typename T, int arr_size>
bool Tree<T, arr_size>::remove(const T &element) {
    return false;
}

template<typename T, int arr_size>
T Tree<T, arr_size>::get_by_index(int index) const {
    return static_cast<T>(index);
}

template<typename T, int arr_size>
T Tree<T, arr_size>::operator[](const int index) const {
    return get_by_index(index);
}

template<typename T, int arr_size>
bool Tree<T, arr_size>::sort() {
    return false;
}

template<typename T, int arr_size>
bool Tree<T, arr_size>::balance() {
    return false;
}

template<typename T, int arr_size>
void Tree<T, arr_size>::save_to_binary_file(const std::ofstream &ofs) const {}

template<typename T, int arr_size>
void Tree<T, arr_size>::load_from_binary_file(std::ifstream &ifs) {}

template<typename T, int arr_size>
std::string Tree<T, arr_size>::to_string() {
    std::ostringstream oss;
    auto current = std::dynamic_pointer_cast<TreeNode>(root);
    if (!current) {
        oss << "Empty Tree";
        return oss.str();
    }
    traverse(root, [&oss](const std::shared_ptr<TreeNode> &node) {
        oss << node->to_string();
    });
    return oss.str();
}
