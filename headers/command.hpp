#ifndef PO_COMMAND_HPP
#define PO_COMMAND_HPP
#include "util.hpp"
#include "option.hpp"
#include "positional.hpp"
#include <tuple>

namespace program_options {
template <std::size_t N>
bool constexpr has_option(std::tuple<> t, const char (&s)[N]) {
    return false;
}

template <std::size_t N, typename Tuple>
bool constexpr has_option(Tuple t, const char (&s)[N]) {
    if (streq(std::remove_reference<decltype(std::get<0>(t))>::type::name::name, s)) return true;
    return has_option(tail(t), s);
}

template <typename Tuple>
bool constexpr has_duplicates(Tuple t) {
    if (has_option(tail(t), std::remove_reference<decltype(std::get<0>(t))>::type::name::name))
        return true;
    return has_duplicates(tail(t));
}

template <>
bool constexpr has_duplicates(std::tuple<> t) {
    return false;
}

template<typename...>
class command {};

template<typename opt, typename... Options, typename... Positionals, typename... Commands>
class command<opt, std::tuple<Options...>, std::tuple<Positionals...>, std::tuple<Commands...>> {
    std::tuple<Options...> options;
    std::tuple<Positionals...> positionals;
    std::tuple<Commands...> commands;
public:
    typedef opt name;
    constexpr command(
        const std::tuple<Options...>& options,
        const std::tuple<Positionals...>& positionals,
        const std::tuple<Commands...>& commands):
        options(options), positionals(positionals), commands(commands) {
        static_assert(sizeof...(Commands) == 0 || sizeof...(Positionals) == 0, "You cannot have both positional arguments and subcommands!");
        CHECK(!has_duplicates(options), "You cannot give duplicated options!");
        CHECK(!has_duplicates(positionals), "You cannot give duplicated positional arguments!");
        CHECK(!has_duplicates(commands), "You cannot give duplicated subcommands!");
    }
    std::ostream& help_line(std::ostream& out) const {
        out << "       ";
        opt::write_long_name(out);
        out << "    ";
        opt::write_description(out);
        return out;
    }
    void print_help(std::ostream& out) const {
        out << "Usage: ";
        opt::write_long_name(out, false);
        out << " ";
        if (sizeof...(Options) > 0)
            out << "[options] ";
        for_each(positionals, [&out](auto p) {
            for (unsigned i=0; i<decltype(p)::min_num; i++) {
                decltype(p)::name::write_long_name(out, false);
                out << " ";
            }
            if (decltype(p)::min_num != decltype(p)::max_num) {
                out << "[";
                decltype(p)::name::write_long_name(out, false);
                if (decltype(p)::min_num + 1 != decltype(p)::max_num) out << " ...] ";
                else out << "] ";
            }
        });
        if (sizeof...(Commands) > 0)
            out << "subcommand";
        out << std::endl;
        if (sizeof...(Options) + sizeof...(Positionals) > 0) {
            out << "Available options and arguments:" << std::endl;
            for_each(options, [&out](auto o) {
                o.help_line(out) << std::endl;
            });
            for_each(positionals, [&out](auto p) {
                p.help_line(out) << std::endl;
            });
        }
        if (sizeof...(Commands) > 0) {
            out << "Available subcommands:" << std::endl;
            for_each(commands, [&out](auto c) {
                c.help_line(out) << std::endl;
            });
        }
    }
};

template<
    typename opt,
    typename... Options,
    typename... Positionals,
    typename... Commands>
constexpr auto create_command_impl(
    std::size_t vpos,
    const std::tuple<Options...>& opts,
    const std::tuple<Positionals...>& pos,
    const std::tuple<Commands...>& coms
) {
    CHECK(vpos <= 1, "You cannot have multiple positional arguments with a non-fixed number of occurences!");
    return command<opt, std::tuple<Options...>, std::tuple<Positionals...>, std::tuple<Commands...>>(opts, pos, coms);
}

template<
    typename opt,
    typename... InnerArgs,
    typename... Args,
    typename... Options,
    typename... Positionals,
    typename... Commands>
constexpr auto create_command_impl(
    std::size_t vpos,
    const std::tuple<Options...>& opts,
    const std::tuple<Positionals...>& pos,
    const std::tuple<Commands...>& coms,
    const option<InnerArgs...>& o,
    const Args&... args
) {
    return create_command_impl<opt>(vpos, std::tuple_cat(opts, std::make_tuple(o)), pos, coms, args...);
}

template<
    typename opt,
    typename pos_opt,
    typename T,
    std::size_t low,
    std::size_t up,
    typename... Args,
    typename... Options,
    typename... Positionals,
    typename... Commands>
constexpr auto create_command_impl(
    std::size_t vpos,
    const std::tuple<Options...>& opts,
    const std::tuple<Positionals...>& pos,
    const std::tuple<Commands...>& coms,
    const positional<pos_opt, T, low, up>& p,
    const Args&... args
) {
    return create_command_impl<opt>(vpos + (low != up), opts, std::tuple_cat(pos, std::make_tuple(p)), coms, args...);
}

template<
    typename opt,
    typename... InnerArgs,
    typename... Args,
    typename... Options,
    typename... Positionals,
    typename... Commands>
constexpr auto create_command_impl(
    std::size_t vpos,
    const std::tuple<Options...>& opts,
    const std::tuple<Positionals...>& pos,
    const std::tuple<Commands...>& coms,
    const command<InnerArgs...>& c,
    const Args&... args
) {
    return create_command_impl<opt>(vpos, opts, pos, std::tuple_cat(coms, std::make_tuple(c)), args...);
}

template<typename opt, typename... Args>
constexpr auto create_command(const Args&... args) {
    return create_command_impl<opt>(0, std::tuple<>(), std::tuple<>(), std::tuple<>(), args...);
}

} // namespace program_options
#endif
