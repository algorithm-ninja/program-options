#include "headers/option.hpp"
#include "headers/positional.hpp"
#include "headers/command.hpp"
#include <iostream>

namespace program_options {
DEFINE_OPTION(help, "This message", 'h');
DEFINE_OPTION(prova3, "Argument repeated three times");
DEFINE_OPTION(prova1to2, "Argument repeated 1 to 2 times");
DEFINE_OPTION(prova1to3, "Argument repeated 1 to 3 times");
DEFINE_OPTION(prova1to4, "Argument repeated 1 to 4 times");
DEFINE_OPTION(box_id, "ID of the sandbox", 'b');
DEFINE_OPTION(root_path, "Path where we will put the sandboxes", 'r');
DEFINE_OPTION(void_option, "Option without any argument", 'v');
DEFINE_OPTION(defaulted_option, "Option with default argument", 'd');

DEFINE_COMMAND(build, "Build subcommand",
    positional<_prova3, int, 3>(),
    positional<_prova1to2, int, 1, 2>(),
//  positional<_prova1to4, int, 1, 4>(),
    option<_help, void>(),
    option<_box_id, int>());

DEFINE_COMMAND(main, "Main command",
    option<_box_id, int>(),
    option<_root_path, int>(),
    option<_defaulted_option, int>(10),
//    option<_defaulted_option, int>(10),
    option<_help, void>(),
    option<_void_option, void>(),
    &build_command); //TODO: if at all possible, remove & from here.

template<>
void command_callback(const decltype(main_command)& mc) {
    if (mc.has_option<_help>()) {
        mc.print_help(std::cerr);
        exit(1);
    }
}

template<>
void command_callback(const decltype(main_command)& mc, const decltype(build_command)& bc) {
    if (bc.has_option<_help>()) {
        bc.print_help(std::cerr);
        exit(1);
    }
}
} // namespace program_options

int main(int argc, const char** argv) {
    program_options::main_command.parse(argc, argv);
}
