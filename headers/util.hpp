#ifndef PO_UTIL_HPP
#define PO_UTIL_HPP
#include <string>
#include <stdexcept>
#include <utility>
#include <tuple>

#define CHECK(cond, msg) (cond) || (throw std::logic_error(msg), true)

class parse_error: public std::runtime_error {
public:
    parse_error(const char* msg): std::runtime_error(msg) {}
};

template<typename T>
T from_char_ptr(const char* ptr) {
    return ptr;
}

template<>
int from_char_ptr(const char* ptr) {
    return std::stoi(ptr);
}

template<>
long from_char_ptr(const char* ptr) {
    return std::stol(ptr);
}

template<>
long long from_char_ptr(const char* ptr) {
    return std::stoll(ptr);
}

template<>
unsigned from_char_ptr(const char* ptr) {
    return std::stoul(ptr);
}

template<>
unsigned long from_char_ptr(const char* ptr) {
    return std::stoul(ptr);
}

template<>
unsigned long long from_char_ptr(const char* ptr) {
    return std::stoull(ptr);
}

template<>
float from_char_ptr(const char* ptr) {
    return std::stof(ptr);
}

template<>
double from_char_ptr(const char* ptr) {
    return std::stod(ptr);
}

template<>
long double from_char_ptr(const char* ptr) {
    return std::stold(ptr);
}

static constexpr bool is_lower_letter(const char c) {
    return 'a' <= c && c <= 'z';
}
static constexpr bool is_upper_letter(const char c) {
    return 'A' <= c && c <= 'Z';
}
static constexpr bool is_number(const char c) {
    return '0' <= c && c <= '9';
}
template<std::size_t N>
static constexpr bool is_long_name_valid(const char (&ln)[N]) {
    if (!is_lower_letter(ln[0])) return false;
    for (unsigned i=0; i<N-1; i++)
        if (!(is_lower_letter(ln[i]) || is_upper_letter(ln[i]) || is_number(ln[i]) || ln[i] == '_'))
            return false;
    if (ln[N-2] == '_') return false;
    return true;
}
static constexpr bool is_short_name_valid(char sn) {
    return is_lower_letter(sn) || is_upper_letter(sn) || sn == 0;
}
template<std::size_t l>
static constexpr std::size_t strlen(const char (&s)[l]) {
    return l;
}

#define DEFINE_OPTION_3(optn, optd, opts) \
struct _##optn { \
    static constexpr const char name[] = #optn;\
    static constexpr const char description[] = optd;\
    static constexpr const char short_name = opts;\
    static constexpr const size_t max_width = 20;\
    _##optn() { \
        static_assert(sizeof(name) <= max_width, "Name too long!");\
        static_assert(is_long_name_valid(name), "Invalid name given!");\
        static_assert(is_short_name_valid(short_name), "Invalid short name given!");\
    } \
    static void write_short_name(std::ostream& out) { \
        out << opts; \
    } \
    static void write_description(std::ostream& out) { \
        out << optd; \
    } \
    static void write_long_name(std::ostream& out, bool pad = true) { \
        for (unsigned i=0; i<sizeof(#optn); i++) { \
            if (#optn[i] != '_') out << #optn[i]; \
            else out << '-';\
        } \
        for (unsigned i=sizeof(#optn); i<max_width*pad; i++) \
            out << ' ';\
    } \
};

#define DEFINE_OPTION_2(on, od) DEFINE_OPTION_3(on, od, 0)

#define GET_MACRO(_1,_2,_3,NAME,...) NAME
#define DEFINE_OPTION(...) GET_MACRO(__VA_ARGS__, DEFINE_OPTION_3, DEFINE_OPTION_2)(__VA_ARGS__)

template <typename Tuple, typename F, std::size_t ...Indices>
void for_each_impl(Tuple&& tuple, F&& f, std::index_sequence<Indices...>) {
    using swallow = int[];
    (void)swallow{1,
        (f(std::get<Indices>(std::forward<Tuple>(tuple))), void(), int{})...
    };
}

template <typename Tuple, typename F>
void for_each(Tuple&& tuple, F&& f) {
    constexpr std::size_t N = std::tuple_size<std::remove_reference_t<Tuple>>::value;
    for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
                  std::make_index_sequence<N>{});
}
#endif
