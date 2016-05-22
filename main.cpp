#include "headers/option.hpp"
#include <type_traits>

int main() {
    constexpr option<int> opt1("box-id", 'b');
    constexpr option<int> opt2("root-path", 'r');
    //constexpr option<int> opt3("root_path", 'r');
    //constexpr option<int> opt4("Pippo", 'P');
    constexpr option<int> opt5("pippo", 'p', 10);
    constexpr option<int> opt6("pluto", 0, 10);
    constexpr option<int> opt7("pluto_", 0, 10);
}
