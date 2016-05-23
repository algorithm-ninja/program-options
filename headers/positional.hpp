#ifndef PO_POSITIONAL_HPP
#define PO_POSITIONAL_HPP
#include "util.hpp"
#include <vector>
#include <array>

// We assume that specifying up != low means we want an arbitrary number of arguments,
// and use up as the maximum number of parameters we want to support.

template<typename T, std::size_t low = 1, std::size_t up = low>
class positional {
    const char* name;
    const char* description;
    std::array<T, up> values;
public:
    template<std::size_t N, std::size_t M>
    constexpr positional(const char (&name)[N], const char (&description)[M]):
        name(name), description(description), values() {
        CHECK(is_long_name_valid(name), "Invalid name given!");
        static_assert(low <= up, "The lower bound on the number of argument must not be greater than the upper bound!");
    }
};
#endif
