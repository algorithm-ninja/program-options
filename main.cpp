#include "headers/option.hpp"
#include <type_traits>

int main() {
    constexpr option<int> opt1("box-id", "Id of the sandbox", 'b');
    constexpr option<int> opt2("root-path", "Path where we will put the sandboxes", 'r');
    //constexpr option<int> opt3("root_path", "Path where we will put the sandboxes", 'r');
    //constexpr option<int> opt4("Pippo", "Path where we will put the sandboxes", 'r');
    constexpr option<int> opt5("pippo", "blabla", 'p', 10);
    constexpr option<int> opt6("pluto", "blabla", 0, 10);
    //constexpr option<int> opt7("pluto_", "blabla", 0, 10);
    constexpr option<int> opt8("pluto", "blabla");
    constexpr option<void> opt9("pluto", "blabla");
    constexpr option<void> opt10("pluto", "blabla", 'c');
}
