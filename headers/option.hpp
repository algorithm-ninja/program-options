#ifndef PO_OPTION_HPP
#define PO_OPTION_HPP
#include "util.hpp"
#include <ostream>
#include <iomanip>
#include <iostream>

namespace program_options {

template<typename opt, typename T>
class option {
    T value;
    T default_value;
public:
    static const int skip_count = 1;
    bool has_default_value = false;
    bool parsed = false;
    typedef opt name;
    constexpr option(): value(), default_value() {}
    constexpr option(T default_value): value(), default_value(default_value), has_default_value(true) {}
    std::ostream& help_line(std::ostream& out) const {
        if (opt::short_name) {
            out << " -";
            opt::write_short_name(out);
            out << ", --";
        } else {
            out << "     --";
        }
        opt::write_long_name(out);
        out << "    ";
        opt::write_description(out);
        return out;
    }
    void parse(const char* arg) {
        value = from_char_ptr<T>(arg);
        parsed = true;
    }
    T get() const {
        if (parsed) return value;
        if (has_default_value) return default_value;
        throw std::logic_error("You cannot get an option that was not parsed!");
    }
};

template<typename opt>
class option<opt, void> {
public:
    static const int skip_count = 0;
    bool has_default_value = true;
    bool parsed = false;
    typedef opt name;
    constexpr option(){}
    std::ostream& help_line(std::ostream& out) const {
        if (opt::short_name) {
            out << " -";
            opt::write_short_name(out);
            out << ", --";
        } else {
            out << "     --";
        }
        opt::write_long_name(out);
        out << "    ";
        opt::write_description(out);
        return out;
    }
    void parse(const char* arg) {
        parsed = true;
    }
    constexpr void get() const {
        throw std::logic_error("You cannot get an option with no argument!");
    }
};

} // namespace program_options
#endif
