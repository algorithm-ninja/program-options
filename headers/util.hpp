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

static constexpr bool is_lower_letter(const char c) {
    return 'a' <= c && c <= 'z';
}
static constexpr bool is_upper_letter(const char c) {
    return 'A' <= c && c <= 'Z';
}
static constexpr bool is_number(const char c) {
    return '0' <= c && c <= '9';
}
template<std::size_t N>
static constexpr bool is_long_name_valid(const char (&ln)[N]) {
    if (!is_lower_letter(ln[0])) return false;
    for (unsigned i=0; i<N-1; i++)
        if (!(is_lower_letter(ln[i]) || is_upper_letter(ln[i]) || is_number(ln[i]) || ln[i] == '-'))
            return false;
    return true;
}
static constexpr bool is_short_name_valid(char sn) {
    return is_lower_letter(sn) || is_upper_letter(sn) || sn == 0;
}

#endif
