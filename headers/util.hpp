#ifndef PO_UTIL_HPP
#define PO_UTIL_HPP
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>

namespace program_options {
#define CHECK(cond, msg) (cond) || (throw std::logic_error(msg), true)

class parse_error : public std::runtime_error {
    std::string message;

  public:
    explicit parse_error(const char* msg)
        : std::runtime_error(""), message(msg) {}
    explicit parse_error(std::string s)
        : std::runtime_error(""), message(std::move(s)) {}
    const char* what() const noexcept override { return message.c_str(); }
};

template <typename T>
static inline T from_char_ptr(const char* ptr) {
    return T(ptr);
}

template <>
int from_char_ptr(const char* ptr) __attribute((unused));  // NOLINT

template <>
int from_char_ptr(const char* ptr) {  // NOLINT
    std::size_t cnt;
    auto res = std::stoi(ptr, &cnt);
    if (ptr[cnt] != 0) {
        throw parse_error("Invalid integer given!");
    }
    return res;
}

template <>
long from_char_ptr(const char* ptr) __attribute((unused));  // NOLINT

template <>
long from_char_ptr(const char* ptr) {  // NOLINT
    std::size_t cnt;
    auto res = std::stol(ptr, &cnt);
    if (ptr[cnt] != 0) {
        throw parse_error("Invalid long given!");
    }
    return res;
}

template <>
long long from_char_ptr(const char* ptr) __attribute((unused));  // NOLINT

template <>
long long from_char_ptr(const char* ptr) {  // NOLINT
    std::size_t cnt;
    auto res = std::stoll(ptr, &cnt);
    if (ptr[cnt] != 0) {
        throw parse_error("Invalid long long given!");
    }
    return res;
}

template <>
unsigned from_char_ptr(const char* ptr) __attribute((unused));  // NOLINT

template <>
unsigned from_char_ptr(const char* ptr) {  // NOLINT
    std::size_t cnt;
    auto res = std::stoul(ptr, &cnt);
    if (ptr[cnt] != 0) {
        throw parse_error("Invalid unsigned given!");
    }
    return res;
}

template <>
unsigned long from_char_ptr(const char* ptr) __attribute((unused));  // NOLINT

template <>
unsigned long from_char_ptr(const char* ptr) {  // NOLINT
    std::size_t cnt;
    auto res = std::stoul(ptr, &cnt);
    if (ptr[cnt] != 0) {
        throw parse_error("Invalid unsigned long given!");
    }
    return res;
}

template <>
unsigned long long from_char_ptr(const char* ptr)
    __attribute((unused));  // NOLINT

template <>
unsigned long long from_char_ptr(const char* ptr) {  // NOLINT
    std::size_t cnt;
    auto res = std::stoull(ptr, &cnt);
    if (ptr[cnt] != 0) {
        throw parse_error("Invalid unsigned long long given!");
    }
    return res;
}

template <>
float from_char_ptr(const char* ptr) __attribute((unused));

template <>
float from_char_ptr(const char* ptr) {
    std::size_t cnt;
    auto res = std::stof(ptr, &cnt);
    if (ptr[cnt] != 0) {
        throw parse_error("Invalid float given!");
    }
    return res;
}

template <>
double from_char_ptr(const char* ptr) __attribute((unused));

template <>
double from_char_ptr(const char* ptr) {
    std::size_t cnt;
    auto res = std::stod(ptr, &cnt);
    if (ptr[cnt] != 0) {
        throw parse_error("Invalid double given!");
    }
    return res;
}

template <>
long double from_char_ptr(const char* ptr) __attribute((unused));

template <>
long double from_char_ptr(const char* ptr) {
    std::size_t cnt;
    auto res = std::stold(ptr, &cnt);
    if (ptr[cnt] != 0) {
        throw parse_error("Invalid integer given!");
    }
    return res;
}

static constexpr bool is_lower_letter(const char c) {
    return 'a' <= c && c <= 'z';
}
static constexpr bool is_upper_letter(const char c) {
    return 'A' <= c && c <= 'Z';
}
static constexpr bool is_number(const char c) { return '0' <= c && c <= '9'; }
template <std::size_t N>
static constexpr bool is_long_name_valid(const char (&ln)[N]) {
    if (!is_lower_letter(ln[0])) return false;
    for (unsigned i = 0; i < N - 1; i++) {
        if (!(is_lower_letter(ln[i]) || is_upper_letter(ln[i]) ||
              is_number(ln[i]) || ln[i] == '_')) {
            return false;
        }
    }
    return ln[N - 2] != '_';
}
static constexpr bool is_short_name_valid(char sn) {
    return is_lower_letter(sn) || is_upper_letter(sn) || sn == 0;
}
template <std::size_t l>
static constexpr std::size_t strlen(const char (&s)[l]) {
    return l;
}

template <std::size_t N, std::size_t M>
static constexpr bool streq(const char (&s1)[N], const char (&s2)[M]) {
    if (N != M) return false;
    for (unsigned i = 0; i < N; i++) {
        if (s1[i] != s2[i]) return false;
    }
    return true;
}

#define DEFINE_OPTION_3(optn, optd, opts)                                   \
    struct _##optn {                                                        \
        static constexpr const char name[] = #optn;                         \
        static constexpr const char description[] = optd;                   \
        static constexpr const char short_name = opts;                      \
        static constexpr const size_t max_width = 20;                       \
        _##optn() {                                                         \
            static_assert(sizeof(name) <= max_width, "Name too long!");     \
            static_assert(is_long_name_valid(name), "Invalid name given!"); \
            static_assert(is_short_name_valid(short_name),                  \
                          "Invalid short name given!");                     \
        }                                                                   \
        static void write_short_name(std::ostream& out) { out << (opts); }  \
        static void write_description(std::ostream& out) { out << (optd); } \
        static void write_long_name(std::ostream& out, bool pad = true) {   \
            for (unsigned i = 0; i < sizeof(#optn); i++) {                  \
                if (#optn[i] != '_')                                        \
                    out << #optn[i];                                        \
                else                                                        \
                    out << '-';                                             \
            }                                                               \
            for (unsigned i = sizeof(#optn); i < max_width * pad; i++)      \
                out << ' ';                                                 \
        }                                                                   \
        static bool matches(const char* ptr) {                              \
            for (unsigned i = 0; i < sizeof(#optn); i++)                    \
                if (ptr[i] != #optn[i]) return false;                       \
            return true;                                                    \
        }                                                                   \
        static bool matches(const char shn) { return shn == (opts); }       \
        static constexpr const char* get_name() { return #optn; }           \
        static constexpr const char* get_description() { return (optd); }   \
        static constexpr const char get_short_name() { return (opts); }     \
    };

#define DEFINE_OPTION_2(on, od) DEFINE_OPTION_3(on, od, 0)

#define GET_MACRO(_1, _2, _3, NAME, ...) NAME
#define DEFINE_OPTION(...) \
    GET_MACRO(__VA_ARGS__, DEFINE_OPTION_3, DEFINE_OPTION_2)(__VA_ARGS__)

template <typename Tuple, typename F, std::size_t... Indices>
void for_each_impl(Tuple&& tuple, F&& f, std::index_sequence<Indices...>) {
    using swallow = int[];
    (void)swallow{1, (f(std::get<Indices>(std::forward<Tuple>(tuple))), void(),
                      int{})...};
}

template <typename Tuple, typename F>
void for_each(Tuple&& tuple, F&& f) {
    constexpr std::size_t N =
        std::tuple_size<std::remove_reference_t<Tuple>>::value;
    for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
                  std::make_index_sequence<N>{});
}

template <typename Tuple, typename F, std::size_t... Indices>
void for_each_impl_rev(Tuple&& tuple, F&& f, std::index_sequence<Indices...>) {
    using swallow = int[];
    (void)swallow{1, (f(std::get<sizeof...(Indices)-Indices - 1>(
                          std::forward<Tuple>(tuple))),
                      void(), int{})...};
}

template <typename Tuple, typename F>
void for_each_rev(Tuple&& tuple, F&& f) {
    constexpr std::size_t N =
        std::tuple_size<std::remove_reference_t<Tuple>>::value;
    for_each_impl_rev(std::forward<Tuple>(tuple), std::forward<F>(f),
                      std::make_index_sequence<N>{});
}

template <std::size_t... Ns, typename... Ts>
auto constexpr tail_impl(std::index_sequence<Ns...>, std::tuple<Ts...> t) {
    return std::make_tuple(std::get<Ns + 1u>(t)...);
}

template <typename... Ts>
auto constexpr tail(std::tuple<Ts...> t) {
    return tail_impl(std::make_index_sequence<sizeof...(Ts)-1u>(), t);
}

template <typename Type>
struct cleanup {
    typedef typename std::remove_pointer<
        typename std::remove_reference<Type>::type>::type type;
};

template <typename T>
bool constexpr dFalse() {
    return false;
}
}  // namespace program_options

#endif
