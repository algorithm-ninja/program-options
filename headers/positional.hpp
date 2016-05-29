#ifndef PO_POSITIONAL_HPP
#define PO_POSITIONAL_HPP
#include "util.hpp"
#include <vector>
#include <array>
#include <iostream>

namespace program_options {

// We assume that specifying up != low means we want an arbitrary number of arguments,
// and use up as the maximum number of parameters we want to support.

template<typename opt, typename T, std::size_t low = 1, std::size_t up = low>
class positional {
    std::array<T, up> values;
public:
    typedef opt name;
    static constexpr size_t min_num = low;
    static constexpr size_t max_num = up;
    size_t parsed_num = 0;
    constexpr positional(): values() {
        static_assert(low <= up, "The lower bound on the number of argument must not be greater than the upper bound!");
    }
    std::ostream& help_line(std::ostream& out) const {
        out << "       ";
        opt::write_long_name(out);
        out << "    ";
        opt::write_description(out);
        return out;
    }
    void parse(const char** first, const char** last) {
        if (last - first > (long long) up)
            throw parse_error("Too many positional arguments given!");
        for (size_t i=0; first+i < last; i++) {
            values[i] = from_char_ptr<T>(first[i]);
            parsed_num++;
        }
    }
    void check() const {
        if (parsed_num < low)
            throw parse_error("Some required positional arguments for " + std::string(opt::get_name()) + " are missing!");
    }
    std::vector<T> get() const {
        return {values.begin(), values.begin()+parsed_num};
    }
};

} // namespace program_options
#endif
