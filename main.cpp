#include "headers/option.hpp"
#include "headers/positional.hpp"
#include "headers/command.hpp"
#include <iostream>

DEFINE_OPTION(build, "Build subcommand");
DEFINE_OPTION(prova3, "Argument repeated three times");
DEFINE_OPTION(prova1to3, "Argument repeated 1 to 3 times");
DEFINE_OPTION(prova1to4, "Argument repeated 1 to 4 times");
DEFINE_OPTION(box_id, "ID of the sandbox", 'b');
DEFINE_OPTION(main, "Main command");
DEFINE_OPTION(root_path, "Path where we will put the sandboxes", 'r');
DEFINE_OPTION(void_option, "Option without any argument", 'v');
DEFINE_OPTION(defaulted_option, "Option with default argument", 'd');

int main() {
    constexpr auto subcommand = create_command<_build>(
        positional<_prova3, int, 3>(),
        positional<_prova1to3, int, 1, 3>(),
//        positional<_prova1to4, int, 1, 4>(),
        option<_box_id, int>());
        
    constexpr auto command = create_command<_main>(
        option<_box_id, int>(),
        option<_root_path, int>(),
        option<_defaulted_option, int>(10),
        option<_void_option, void>(),
        subcommand);
    subcommand.print_help(std::cout);
    std::cout << std::endl << std::endl;
    command.print_help(std::cout);
}
