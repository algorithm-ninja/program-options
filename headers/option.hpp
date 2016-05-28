#ifndef PO_OPTION_HPP
#define PO_OPTION_HPP
#include "util.hpp"
#include <ostream>
#include <iomanip>

template<typename opt, typename T>
class option {
    T value;
    T default_value;
    bool has_default_value = false;
    bool parsed = false;
    static const int skip_count = 1;
public:
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
};

template<typename opt>
class option<opt, void> {
    static const int skip_count = 0;
    bool parsed = false;
public:
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
};

#endif
