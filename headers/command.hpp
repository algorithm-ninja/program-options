#ifndef PO_COMMAND_HPP
#define PO_COMMAND_HPP
#include <iostream>
#include <tuple>
#include "option.hpp"
#include "positional.hpp"
#include "util.hpp"

namespace program_options {
template <std::size_t N>
bool constexpr has_option(std::tuple<> t, const char (&s)[N]) {
    return false;
}

template <std::size_t N, typename Tuple>
bool constexpr has_option(Tuple t, const char (&s)[N]) {
    if (streq(cleanup<decltype(std::get<0>(t))>::type::name::name, s))
        return true;
    return has_option(tail(t), s);
}

template <typename Tuple>
bool constexpr has_duplicates(Tuple t) {
    if (has_option(tail(t),
                   cleanup<decltype(std::get<0>(t))>::type::name::name))
        return true;
    return has_duplicates(tail(t));
}

template <>
bool constexpr has_duplicates(std::tuple<> t) {
    return false;
}

template <typename Tuple>
bool constexpr has_short_option(Tuple t, const char s) {
    if (!s) return false;
    if (cleanup<decltype(std::get<0>(t))>::type::name::short_name == s)
        return true;
    return has_short_option(tail(t), s);
}

template <>
bool constexpr has_short_option(std::tuple<> t, const char s) {
    return false;
}

template <typename Tuple>
bool constexpr has_short_duplicates(Tuple t) {
    if (has_short_option(
            tail(t), cleanup<decltype(std::get<0>(t))>::type::name::short_name))
        return true;
    return has_short_duplicates(tail(t));
}

template <>
bool constexpr has_short_duplicates(std::tuple<> t) {
    return false;
}

template <typename T, typename Tuple>
struct get_with_name_helper {
    static size_t constexpr value =
        streq(cleanup<decltype(std::get<0>(Tuple()))>::type::name::name,
              T::name)
            ? 0
            : get_with_name_helper<T, decltype(tail(Tuple()))>::value + 1;
};

template <typename T>
struct get_with_name_helper<T, std::tuple<>> {
    static constexpr size_t value = -1;
    constexpr get_with_name_helper() {
        CHECK(false, "There is no option with this name!");
    }
};

template <typename T, typename Tuple>
const auto& get_with_name(const Tuple& t) {
    return std::get<get_with_name_helper<T, Tuple>::value>(t);
}

template <typename T, typename... Args>
void command_callback(const T&, const Args&...) {
    static_assert(dFalse<T>(), "Some (sub)command has no callback!");
}

template <typename...>
class command {};

// TODO: add a way to retreive the used subcommand.
template <typename opt, typename... Options, typename... Positionals,
          typename... Commands>
class command<opt, std::tuple<Options...>, std::tuple<Positionals...>,
              std::tuple<Commands...>> {
    std::tuple<Options...> options;
    std::tuple<Positionals...> positionals;
    std::tuple<Commands*...> commands;
    int parse_option(int argc, const char** argv) {
        CHECK(argv[0][0] == '-', "Invalid option given!");
        int skip = 0;
        if (argv[0][1] == '-') {
            bool parsed = false;
            for_each(options, [&](auto& o) {
                if (!cleanup<decltype(o)>::type::name::matches(argv[0] + 2))
                    return;
                CHECK(!parsed,
                      "Something went wrong - an option was recognized twice!");
                parsed = true;
                skip = cleanup<decltype(o)>::type::skip_count;
                if (argc < skip + 1) {
                    throw parse_error("Missing required argument for option " +
                                      std::string(argv[0] + 2));
                }
                o.parse(argv[skip]);
            });
            if (!parsed) {
                throw parse_error("Unknown option " + std::string(argv[0]) +
                                  " given!");
            }
        } else {
            for (const char* cur = argv[0] + 1; *cur != 0; cur++) {
                bool parsed = false;
                for_each(options, [&](auto& o) {
                    if (!cleanup<decltype(o)>::type::name::matches(*cur))
                        return;
                    CHECK(!parsed,
                          "Something went wrong - a short option was "
                          "recognized twice!");
                    parsed = true;
                    skip = cleanup<decltype(o)>::type::skip_count;
                    if (skip && !o.has_default_value && argv[0][2] != 0) {
                        throw parse_error(
                            "An option that requires an argument was given in "
                            "an option group!");
                    }
                    if (argc < skip + 1) {
                        throw parse_error("Missing required argument for option " +
                                          std::string(argv[0] + 2));
                    }
                    o.parse(argv[skip]);
                });
                if (!parsed) {
                    throw parse_error("Unknown option -" + std::string({*cur}) +
                                      " given!");
                }
            }
        }
        return skip;
    }
    void check_positional() {
        for_each(positionals, [&](const auto& p) { p.check(); });
    }
    template <typename... Supercommands>
    void parse_with_positional(int argc, const char** argv,
                               const Supercommands&... sc) {
        std::vector<const char*> posargs;
        bool positional_only = false;
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] == '-' && !positional_only) {
                // -- marks the end of options
                if (argv[i][1] == '-' && argv[i][2] == 0) {
                    positional_only = true;
                    continue;
                }
                // Don't treat a single dash as an option.
                if (argv[i][1] != 0) {
                    i += parse_option(argc - i, argv + i);
                    continue;
                }
            }
            posargs.emplace_back(argv[i]);
        }
        std::size_t head_count = 0;
        bool found_variadic = false;
        for_each(positionals, [&](auto& p) {
            if (cleanup<decltype(p)>::type::min_num !=
                cleanup<decltype(p)>::type::max_num) {
                found_variadic = true;
            }
            if (found_variadic) return;
            std::size_t next_head_count =
                head_count + cleanup<decltype(p)>::type::min_num;
            if (next_head_count > posargs.size()) {
                throw parse_error("Too few positional arguments given!");
            }
            p.parse(posargs.data() + head_count,
                    posargs.data() + next_head_count);
            head_count = next_head_count;
        });
        if (!found_variadic) {
            if (head_count < posargs.size()) {
                throw parse_error("Too many positional arguments given!");
            }
            check_positional();
            command_callback(sc..., *this);
            return;
        }
        found_variadic = false;
        std::size_t tail_count = 0;
        for_each_rev(positionals, [&](auto& p) {
            if (cleanup<decltype(p)>::type::min_num !=
                cleanup<decltype(p)>::type::max_num) {
                found_variadic = true;
            }
            if (found_variadic) return;
            std::size_t next_tail_count =
                tail_count + cleanup<decltype(p)>::type::min_num;
            if (next_tail_count > posargs.size()) {
                throw parse_error("Too few positional arguments given!");
            }
            p.parse(posargs.data() + posargs.size() - next_tail_count,
                    posargs.data() + posargs.size() - tail_count);
            tail_count = next_tail_count;
        });
        for_each(positionals, [&](auto& p) {
            if (cleanup<decltype(p)>::type::min_num !=
                cleanup<decltype(p)>::type::max_num) {
                p.parse(posargs.data() + head_count,
                        posargs.data() + posargs.size() - tail_count);
            }
        });
        check_positional();
        command_callback(sc..., *this);
    }
    template <typename... Supercommands>
    void parse_with_subcommand(int argc, const char** argv,
                               const Supercommands&... sc) {
        bool has_subcommand = false;
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] == '-') {
                i += parse_option(argc - i, argv + i);
                continue;
            }
            bool parsed = false;
            for_each(commands, [&](auto& c) {
                if (!cleanup<decltype(c)>::type::name::matches(argv[i])) return;
                CHECK(!parsed,
                      "Something went wrong - a subcommand was recognized "
                      "twice!");
                parsed = true;
                command_callback(sc..., *this);
                c->parse(argc - i, argv + i, sc..., *this);
            });
            if (!parsed) {
                throw parse_error("Unknown subcommand " + std::string(argv[i]) +
                                  " given!");
            }
            has_subcommand = true;
            break;
        }
        if (!has_subcommand)
            throw parse_error("You must specify a subcommand!\n");
    }

  public:
    typedef opt name;
    constexpr command(const std::tuple<Options...>& options,
                      const std::tuple<Positionals...>& positionals,
                      const std::tuple<Commands*...>& commands)
        : options(options), positionals(positionals), commands(commands) {
        static_assert(
            sizeof...(Commands) == 0 || sizeof...(Positionals) == 0,
            "You cannot have both positional arguments and subcommands!");
        CHECK(!has_duplicates(options), "You cannot give duplicated options!");
        CHECK(!has_short_duplicates(options),
              "You cannot give duplicated short options!");
        CHECK(!has_duplicates(positionals),
              "You cannot give duplicated positional arguments!");
        CHECK(!has_duplicates(commands),
              "You cannot give duplicated subcommands!");
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
        if (sizeof...(Options) > 0) {
            out << "[options] ";
        }
        for_each(positionals, [&out](auto p) {
            for (unsigned i = 0; i < decltype(p)::min_num; i++) {
                decltype(p)::name::write_long_name(out, false);
                out << " ";
            }
            if (decltype(p)::min_num != decltype(p)::max_num) {
                out << "[";
                decltype(p)::name::write_long_name(out, false);
                if (decltype(p)::min_num + 1 != decltype(p)::max_num) {
                    out << " ...";
                }
                out << "] ";
            }
        });
        if (sizeof...(Commands) > 0) {
            out << "subcommand";
        }
        out << std::endl;
        if (sizeof...(Options) + sizeof...(Positionals) > 0) {
            out << "Available options and arguments:" << std::endl;
            for_each(options,
                     [&out](auto o) { o.help_line(out) << std::endl; });
            for_each(positionals,
                     [&out](auto p) { p.help_line(out) << std::endl; });
        }
        if (sizeof...(Commands) > 0) {
            out << "Available subcommands:" << std::endl;
            for_each(commands,
                     [&out](auto c) { c->help_line(out) << std::endl; });
        }
    }
    template <typename... Supercommands>
    bool parse(int argc, const char** argv, const Supercommands&... sc) {
        try {
            if (sizeof...(Commands) > 0) {
                parse_with_subcommand(argc, argv, sc...);
            } else {
                parse_with_positional(argc, argv, sc...);
            }
            return true;
        } catch (parse_error& e) {
            std::cerr << "Error parsing the command line: " << e.what();
            std::cerr << std::endl;
            print_help(std::cerr);
            return false;
        }
    }
    template <typename T>
    bool has_option() const {
        return get_with_name<T>(options).parsed;
    }
    template <typename T>
    size_t count_positional() const {
        return get_with_name<T>(positionals).parsed_num;
    }
    template <typename T>
    auto get_option() const {
        return get_with_name<T>(options).get();
    }
    template <typename T>
    auto get_positional() const {
        return get_with_name<T>(positionals).get();
    }
};

template <typename opt, typename... Options, typename... Positionals,
          typename... Commands>
constexpr auto create_command_impl(std::size_t vpos,
                                   const std::tuple<Options...>& opts,
                                   const std::tuple<Positionals...>& pos,
                                   const std::tuple<Commands*...>& coms) {
    CHECK(vpos <= 1,
          "You cannot have multiple positional arguments with a non-fixed "
          "number of occurences!");
    return command<opt, std::tuple<std::decay_t<Options>...>,
                   std::tuple<std::decay_t<Positionals>...>,
                   std::tuple<std::decay_t<Commands>...>>(opts, pos, coms);
}

template <typename opt, typename... InnerArgs, typename... Args,
          typename... Options, typename... Positionals, typename... Commands>
constexpr auto create_command_impl(std::size_t vpos,
                                   const std::tuple<Options...>& opts,
                                   const std::tuple<Positionals...>& pos,
                                   const std::tuple<Commands*...>& coms,
                                   const option<InnerArgs...>& o,
                                   const Args&... args) {
    return create_command_impl<opt>(
        vpos, std::tuple_cat(opts, std::make_tuple(o)), pos, coms, args...);
}

template <typename opt, typename pos_opt, typename T, std::size_t low,
          std::size_t up, typename... Args, typename... Options,
          typename... Positionals, typename... Commands>
constexpr auto create_command_impl(std::size_t vpos,
                                   const std::tuple<Options...>& opts,
                                   const std::tuple<Positionals...>& pos,
                                   const std::tuple<Commands*...>& coms,
                                   const positional<pos_opt, T, low, up>& p,
                                   const Args&... args) {
    return create_command_impl<opt>(vpos + (low != up), opts,
                                    std::tuple_cat(pos, std::make_tuple(p)),
                                    coms, args...);
}

template <typename opt, typename... InnerArgs, typename... Args,
          typename... Options, typename... Positionals, typename... Commands>
constexpr auto create_command_impl(std::size_t vpos,
                                   const std::tuple<Options...>& opts,
                                   const std::tuple<Positionals...>& pos,
                                   const std::tuple<Commands*...>& coms,
                                   command<InnerArgs...>* c,
                                   const Args&... args) {
    std::tuple<std::decay_t<Commands*>..., command<InnerArgs...>*> cmd_tuple = std::tuple_cat(coms, std::make_tuple(c));
    return create_command_impl<opt>(
        vpos, opts, pos, cmd_tuple, args...);
}

template <typename opt, typename... Args>
constexpr auto internal_create_command_impl(const Args&... args) {
    return create_command_impl<opt>(0, std::tuple<>(), std::tuple<>(),
                                    std::tuple<>(), args...);
}

template <typename opt, typename... Args>
constexpr auto internal_create_command(const Args&... args) {
    return internal_create_command_impl<opt, std::decay_t<Args>...>(args...);
}

#define DEFINE_COMMAND(NAME, DESCRIPTION, ...)         \
    DEFINE_OPTION(NAME, DESCRIPTION)                   \
    constexpr auto _po_internal_##NAME##_command =     \
        internal_create_command<_##NAME>(__VA_ARGS__); \
    auto NAME##_command = _po_internal_##NAME##_command;

}  // namespace program_options
#endif
