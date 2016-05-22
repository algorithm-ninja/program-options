#ifndef PO_OPTION_HPP
#define PO_OPTION_HPP
#include "util.hpp"

class option_ {
protected:
    const char* long_name;
    const char short_name;
    const char* description;
    bool parsed = false;
    static constexpr bool is_lower_letter(const char c) {
        return 'a' <= c && c <= 'z';
    }
    static constexpr bool is_upper_letter(const char c) {
        return 'A' <= c && c <= 'Z';
    }
    template<std::size_t N>
    static constexpr bool is_long_name_valid(const char (&ln)[N]) {
        if (!is_lower_letter(ln[0])) return false;
        for (unsigned i=0; i<N-1; i++)
            if (!(is_lower_letter(ln[i]) || is_upper_letter(ln[i]) || ln[i] == '-'))
                return false;
        return true;
    }
    static constexpr bool is_short_name_valid(char sn) {
        return is_lower_letter(sn) || is_upper_letter(sn) || sn == 0;
    }
    template<std::size_t N, std::size_t M>
    constexpr option_(const char (&long_name)[N], const char short_name, const char (&description)[M]):
        long_name(long_name), short_name(short_name), description(description) {
        CHECK(is_long_name_valid(long_name), "Invalid long name given!");
        CHECK(is_short_name_valid(short_name), "Invalid short name given!");
    }
};

template<typename T>
class option: public option_ {
    T value;
    T default_value;
    bool has_default_value = false;
    static const int skip_count = 1;
public:
    template<std::size_t N, std::size_t M>
    constexpr option(const char (&long_name)[N], const char (&description)[M], const char short_name = 0):
        option_(long_name, short_name, description),
        value(), default_value() {}
    template<std::size_t N, std::size_t M>
    constexpr option(const char (&long_name)[N], const char (&description)[M], const char short_name, T default_value):
        option_(long_name, short_name, description),
        value(), default_value(default_value),
        has_default_value(true) {}
};

template<>
class option<void>: public option_ {
    static const int skip_count = 0;
public:
    template<std::size_t N, std::size_t M>
    constexpr option(const char (&long_name)[N], const char (&description)[M], const char short_name = 0):
        option_(long_name, short_name, description) {}
};

#endif
