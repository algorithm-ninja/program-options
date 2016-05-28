#ifndef PO_POSITIONAL_HPP
#define PO_POSITIONAL_HPP
#include "util.hpp"
#include <vector>
#include <array>

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
};

} // namespace program_options
#endif
