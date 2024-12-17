#pragma once
#include <chrono>
#include <fstream>
#include <numeric>
#include <random>
#include <set>
#include <__random/random_device.h>

#include "Tree.h"

template<typename T, size_t arr_size>
class Benchmark {
public:
    Benchmark() = default;

    ~Benchmark() = default;

    void run();

private:
    std::set<T> elements_set;

    void calculate_statistics(const std::vector<double> &times, double &mean, double &mse);

    Tree<T, arr_size> tree;

    std::string check_sort_time(int);

    void generate_elements(int);

    std::string check_insert_time(int);

    std::string check_remove_by_element_time(int);

    std::string check_remove_by_index_time(int);

    std::string check_get_element_time(int);

    std::string check_traversal_time(int);
};

template<typename T, size_t arr_size>
void Benchmark<T, arr_size>::run() {
    const auto filename = "benchmark.txt";
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + std::string(filename));
    }
    for (int i = 5; i < 501; i *= 10) {
        file << "Elements: " << i << std::endl;
        file << check_insert_time(i);
        file << check_sort_time(i);
        file << check_remove_by_element_time(i);
        file << check_remove_by_index_time(i);
        file << check_get_element_time(i);
        file << check_traversal_time(i);
    }
    std::cout <<
            "Benchmark completed successfully.\nPath to file: /Users/skilanet/CLionProjects/KursProga1/cmake-build-debug/benchmark.txt"
            << std::endl;
}

template<typename T, size_t arr_size>
void Benchmark<T, arr_size>::calculate_statistics(const std::vector<double> &times, double &mean, double &mse) {
    double mean_tmp = 0.0;
    for (const auto time: times) {
        mean_tmp += time / static_cast<double>(times.size());
    }
    double mse_tmp = 0.0;
    for (const auto time: times) {
        mse_tmp += (time - mean_tmp) * (time - mean_tmp) / static_cast<double>(times.size());
    }
    mean = mean_tmp;
    mse = sqrt(mse_tmp);
}

template<typename T, size_t arr_size>
std::string Benchmark<T, arr_size>::check_insert_time(const int elements) {
    std::vector<double> times;
    for (int i = 0; i < 15; ++i) {
        generate_elements(elements);
        const auto start = std::chrono::high_resolution_clock::now();
        for (auto element: elements_set) {
            tree.insert(element);
        }
        auto end = std::chrono::high_resolution_clock::now();
        elements_set.clear();
        times.push_back(std::chrono::duration<double, std::milli>(end - start).count());
        tree.clear();
    }
    double mean = 0.0;
    double mse = 0.0;
    calculate_statistics(times, mean, mse);
    std::ostringstream oss;
    oss << "\tOperation - Insert: ";
    oss << " Mean (ms): " << mean << "; MSE (ms): " << mse << std::endl;
    return oss.str();
}

template<typename T, size_t arr_size>
std::string Benchmark<T, arr_size>::check_sort_time(const int elements) {
    std::vector<double> times;
    for (int i = 0; i < 15; ++i) {
        generate_elements(elements);
        for (auto element: elements_set) {
            tree.insert(element);
        }
        const auto start = std::chrono::high_resolution_clock::now();
        tree.sort();
        const auto end = std::chrono::high_resolution_clock::now();
        elements_set.clear();
        tree.clear();
        times.push_back(std::chrono::duration<double, std::milli>(end - start).count());
    }
    double mean = 0.0;
    double mse = 0.0;
    calculate_statistics(times, mean, mse);
    std::ostringstream oss;
    oss << "\tOperation - Sorting: ";
    oss << " Mean (ms): " << mean << "; MSE (ms): " << mse << std::endl;
    return oss.str();
}

template<typename T, size_t arr_size>
void Benchmark<T, arr_size>::generate_elements(const int size) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution distribution(INT32_MIN, INT32_MAX);
    while (elements_set.size() < size) {
        elements_set.insert(distribution(generator));
    }
}

template<typename T, size_t arr_size>
std::string Benchmark<T, arr_size>::check_remove_by_element_time(int elements) {
    std::vector<double> times;
    for (int i = 0; i < 15; ++i) {
        generate_elements(elements);
        for (auto element: elements_set) {
            tree.insert(element);
        }
        auto first = *elements_set.begin();
        auto middle_iter = std::next(elements_set.begin(), elements_set.size() / 2);
        auto middle = *middle_iter;
        int last = *elements_set.end();
        const auto start = std::chrono::high_resolution_clock::now();
        tree.remove(first);
        tree.remove(middle);
        tree.remove(last);
        const auto end = std::chrono::high_resolution_clock::now();
        times.push_back(std::chrono::duration<double, std::milli>(end - start).count());
    }
    double mean = 0.0;
    double mse = 0.0;
    calculate_statistics(times, mean, mse);
    std::ostringstream oss;
    oss << "\tOperation - Removing by element: ";
    oss << " Mean (ms): " << mean << "; MSE (ms): " << mse << std::endl;
    return oss.str();
}

template<typename T, size_t arr_size>
std::string Benchmark<T, arr_size>::check_remove_by_index_time(int elements) {
    std::vector<double> times;
    for (int i = 0; i < 15; ++i) {
        generate_elements(elements);
        for (auto element: elements_set) {
            tree.insert(element);
        }
        const auto start = std::chrono::high_resolution_clock::now();
        tree.remove_by_index(0);
        tree.remove_by_index(elements / 2);
        tree.remove_by_index(elements - 1);
        const auto end = std::chrono::high_resolution_clock::now();
        times.push_back(std::chrono::duration<double, std::milli>(end - start).count());
        elements_set.clear();
        tree.clear();
    }
    double mean = 0.0;
    double mse = 0.0;
    calculate_statistics(times, mean, mse);
    std::ostringstream oss;
    oss << "\tOperation - Removing by index: ";
    oss << " Mean (ms): " << mean << "; MSE (ms): " << mse << std::endl;
    return oss.str();
}

template<typename T, size_t arr_size>
std::string Benchmark<T, arr_size>::check_get_element_time(int elements) {
    std::vector<double> times;
    for (int i = 0; i < 15; ++i) {
        generate_elements(elements);
        for (auto element: elements_set) {
            tree.insert(element);
        }
        const auto start = std::chrono::high_resolution_clock::now();
        tree.get_by_index(0);
        tree.get_by_index(elements / 2);
        tree.get_by_index(elements - 1);
        const auto end = std::chrono::high_resolution_clock::now();
        times.push_back(std::chrono::duration<double, std::milli>(end - start).count());
        elements_set.clear();
        tree.clear();
    }
    double mean = 0.0;
    double mse = 0.0;
    calculate_statistics(times, mean, mse);
    std::ostringstream oss;
    oss << "\tOperation - Getting by index: ";
    oss << " Mean (ms): " << mean << "; MSE (ms): " << mse << std::endl;
    return oss.str();
}

template<typename T, size_t arr_size>
std::string Benchmark<T, arr_size>::check_traversal_time(int elements) {
    std::vector<double> times;
    for (int i = 0; i < 15; ++i) {
        generate_elements(elements);
        for (auto element: elements_set) {
            tree.insert(element);
        }
        const auto start = std::chrono::high_resolution_clock::now();
        tree.in_order_traversal(false);
        const auto end = std::chrono::high_resolution_clock::now();
        times.push_back(std::chrono::duration<double, std::milli>(end - start).count());
        elements_set.clear();
        tree.clear();
    }
    double mean = 0.0;
    double mse = 0.0;
    calculate_statistics(times, mean, mse);
    std::ostringstream oss;
    oss << "\tOperation - Traversal: ";
    oss << " Mean (ms): " << mean << "; MSE (ms): " << mse << std::endl;
    return oss.str();
}
