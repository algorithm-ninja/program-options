#include "headers/option.hpp"
#include "headers/positional.hpp"
#include "headers/command.hpp"

int main() {
    constexpr auto subcommand = create_command(
        "build",
        "Build subcommand",
        positional<int, 3>("prova3", "Argument repeated three times"),
        option<int>("box-id", "Id of the sandbox", 'b'));
        
    constexpr auto command = create_command(
        "main",
        "Main command",
        option<int>("box-id", "Id of the sandbox", 'b'),
        option<int>("root-path", "Path where we will put the sandboxes", 'r'),
//      option<int>("root_path", "Path where we will put the sandboxes", 'r'),
        option<int>("pippo", "blabla", 'p', 10),
        option<void>("prova", "blabla", 'c'),
//      positional<int, 1, 10>("prova1to10", "Argument repeatable up to 10 times"),
        subcommand);
}
