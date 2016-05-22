#ifndef PO_UTIL_HPP
#define PO_UTIL_HPP
#include <string>
#include <stdexcept>

#define CHECK(cond, msg) (cond) || (throw std::logic_error(msg), true)

class parse_error: public std::runtime_error {
public:
    parse_error(const char* msg): std::runtime_error(msg) {}
};

template<typename T>
T from_char_ptr(const char* ptr) {
    return ptr;
}

template<>
int from_char_ptr(const char* ptr) {
    return std::stoi(ptr);
}

template<>
long from_char_ptr(const char* ptr) {
    return std::stol(ptr);
}

template<>
long long from_char_ptr(const char* ptr) {
    return std::stoll(ptr);
}

template<>
unsigned from_char_ptr(const char* ptr) {
    return std::stoul(ptr);
}

template<>
unsigned long from_char_ptr(const char* ptr) {
    return std::stoul(ptr);
}

template<>
unsigned long long from_char_ptr(const char* ptr) {
    return std::stoull(ptr);
}

template<>
float from_char_ptr(const char* ptr) {
    return std::stof(ptr);
}

template<>
double from_char_ptr(const char* ptr) {
    return std::stod(ptr);
}

template<>
long double from_char_ptr(const char* ptr) {
    return std::stold(ptr);
}

#endif
