#ifndef PO_COMMAND_HPP
#define PO_COMMAND_HPP
#include "util.hpp"
#include "option.hpp"
#include "positional.hpp"
#include <tuple>

template<typename...>
class command {};

template<typename... Options, typename... Positionals, typename... Commands>
class command<std::tuple<Options...>, std::tuple<Positionals...>, std::tuple<Commands...>> {
    const char* name;
    const char* description;
    std::tuple<Options...> options;
    std::tuple<Positionals...> positionals;
    std::tuple<Commands...> commands;
public:
    template<std::size_t N, std::size_t M>
    constexpr command(
        const char (&name)[N], const char (&description)[M],
        const std::tuple<Options...>& options,
        const std::tuple<Positionals...>& positionals,
        const std::tuple<Commands...>& commands):
        name(name), description(description),
        options(options), positionals(positionals), commands(commands) {
        CHECK(is_long_name_valid(name), "Invalid name given!");
        static_assert(sizeof...(Commands) == 0 || sizeof...(Positionals) == 0, "You cannot have both positional arguments and subcommands!");
    }
};

template<
    std::size_t N,
    std::size_t M,
    typename... Options,
    typename... Positionals,
    typename... Commands>
constexpr auto create_command_impl(
    const std::tuple<Options...>& opts,
    const std::tuple<Positionals...>& pos,
    const std::tuple<Commands...>& coms,
    const char (&name)[N],
    const char (&description)[M]
) {
    return command<
        std::tuple<Options...>,
        std::tuple<Positionals...>,
        std::tuple<Commands...>
    >(name, description, opts, pos, coms);
}

template<
    std::size_t N,
    std::size_t M,
    typename T,
    typename... Args,
    typename... Options,
    typename... Positionals,
    typename... Commands>
constexpr auto create_command_impl(
    const std::tuple<Options...>& opts,
    const std::tuple<Positionals...>& pos,
    const std::tuple<Commands...>& coms,
    const char (&name)[N],
    const char (&description)[M],
    const option<T>& opt,
    const Args&... args
) {
    return create_command_impl(
        std::tuple_cat(opts, std::make_tuple(opt)), pos, coms,
        name, description, args...);
}

template<
    std::size_t N,
    std::size_t M,
    typename T,
    std::size_t low,
    std::size_t up,
    typename... Args,
    typename... Options,
    typename... Positionals,
    typename... Commands>
constexpr auto create_command_impl(
    const std::tuple<Options...>& opts,
    const std::tuple<Positionals...>& pos,
    const std::tuple<Commands...>& coms,
    const char (&name)[N],
    const char (&description)[M],
    const positional<T, low, up>& p,
    const Args&... args
) {
    return create_command_impl(
        opts, std::tuple_cat(pos, std::make_tuple(p)), coms,
        name, description, args...);
}

template<
    std::size_t N,
    std::size_t M,
    typename... InnerArgs,
    typename... Args,
    typename... Options,
    typename... Positionals,
    typename... Commands>
constexpr auto create_command_impl(
    const std::tuple<Options...>& opts,
    const std::tuple<Positionals...>& pos,
    const std::tuple<Commands...>& coms,
    const char (&name)[N],
    const char (&description)[M],
    const command<InnerArgs...>& com,
    const Args&... args
) {
    return create_command_impl(
        opts, pos, std::tuple_cat(coms, std::make_tuple(com)),
        name, description, args...);
}

template<std::size_t N, std::size_t M, typename... Args>
constexpr auto create_command(
    const char (&name)[N],
    const char (&description)[M], 
    const Args&... args
) {
    return create_command_impl(
        std::tuple<>(), std::tuple<>(), std::tuple<>(),
        name, description, args...);
}

#endif
