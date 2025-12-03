#ifndef LUD_ASSERT_HEADER
#define LUD_ASSERT_HEADER

#include <concepts>
#include <cstdlib>
#include <fcntl.h>

#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include <source_location>

namespace Lud::assert {

void that(bool expr, const std::string_view msg = "", const std::source_location loc = std::source_location::current());

template <class T1, class T2>
    requires std::equality_comparable_with<T1, T2>
void eq(T1 n1, T2 n2, const std::string_view msg = "", const std::source_location loc = std::source_location::current());

template <class T1, class T2>
    requires std::equality_comparable_with<T1, T2>
void ne(T1 n1, T2 n2, const std::string_view msg = "", const std::source_location loc = std::source_location::current());

template <class T1, class T2>
    requires std::totally_ordered_with<T1, T2>
void gt(T1 n1, T2 n2, const std::string_view msg = "", const std::source_location loc = std::source_location::current());

template <class T1, class T2>
    requires std::totally_ordered_with<T1, T2>
void lt(T1 n1, T2 n2, const std::string_view msg = "", const std::source_location loc = std::source_location::current());

template <class T1, class T2>
    requires std::totally_ordered_with<T1, T2>
void geq(T1 n1, T2 n2, const std::string_view msg = "", const std::source_location loc = std::source_location::current());

template <class T1, class T2>
    requires std::totally_ordered_with<T1, T2>
void leq(T1 n1, T2 n2, const std::string_view msg = "", const std::source_location loc = std::source_location::current());

template <class Min, class T, class Max>
    requires std::totally_ordered_with<Min, T> &&
             std::totally_ordered_with<Max, T>
void range(Min min, T val, Max max, const std::string_view msg = "", const std::source_location loc = std::source_location::current());

} // namespace Lud::assert

namespace Lud::check {

inline void that(bool expr, const std::string_view msg = "The expression evaluated to false");

inline void is_false(bool expr, const std::string_view msg = "The expression evaluated to true");

template <class T1, class T2>
    requires std::equality_comparable_with<T1, T2>
void in(T1 n1, std::initializer_list<T2>, const std::string_view msg = "argument is not contained in list");

template <class T1, class T2>
    requires std::equality_comparable_with<T1, T2>
void eq(T1 n1, T2 n2, const std::string_view msg = "passed arguments are not the same");

template <class T1, class T2>
    requires std::equality_comparable_with<T1, T2>
void ne(T1 n1, T2 n2, const std::string_view msg = "passed arguments are the same");

template <class T1, class T2>
    requires std::totally_ordered_with<T1, T2>
void gt(T1 n1, T2 n2, const std::string_view msg = "n1 is not greater than n2");

template <class T1, class T2>
    requires std::totally_ordered_with<T1, T2>
void lt(T1 n1, T2 n2, const std::string_view msg = "n1 is not lower than n2");

template <class T1, class T2>
    requires std::totally_ordered_with<T1, T2>
void geq(T1 n1, T2 n2, const std::string_view msg = "n1 is lower than n2");

template <class T1, class T2>
    requires std::totally_ordered_with<T1, T2>
void leq(T1 n1, T2 n2, const std::string_view msg = "n1 is greater than n2");

template <class Min, class T, class Max>
    requires std::totally_ordered_with<Min, T> &&
             std::totally_ordered_with<Max, T>
void range(Min min, T val, Max max, const std::string_view msg = "val is not in range [min, max)");

} // namespace Lud::check

// https://github.com/nemequ/portable-snippets/blob/master/debug-trap/debug-trap.h

#if defined(__has_builtin) && !defined(__ibmxl__)
    #if __has_builtin(__builtin_debugtrap)
        #define PSNIP_TRAP() __builtin_debugtrap()
    #elif __has_builtin(__debugbreak)
        #define PSNIP_TRAP() __debugbreak()
    #endif
#endif
#if !defined(PSNIP_TRAP)
    #if defined(_MSC_VER) || defined(__INTEL_COMPILER)
        #define PSNIP_TRAP() __debugbreak()
    #elif defined(__ARMCC_VERSION)
        #define PSNIP_TRAP() __breakpoint(42)
    #elif defined(__ibmxl__) || defined(__xlC__)
        #include <builtins.h>
        #define PSNIP_TRAP() __trap(42)
    #elif defined(__DMC__) && defined(_M_IX86)
void PSNIP_TRAP()
{
    __asm int 3h;
}
    #elif defined(__i386__) || defined(__x86_64__)
void PSNIP_TRAP()
{
    __asm__ __volatile__("int3");
}
    #elif defined(__thumb__)
void PSNIP_TRAP()
{
    __asm__ __volatile__(".inst 0xde01");
}
    #elif defined(__aarch64__)
void PSNIP_TRAP()
{
    __asm__ __volatile__(".inst 0xd4200000");
}
    #elif defined(__arm__)
void PSNIP_TRAP()
{
    __asm__ __volatile__(".inst 0xe7f001f0");
}
    #elif defined(__alpha__) && !defined(__osf__)
void PSNIP_TRAP()
{
    __asm__ __volatile__("bpt");
}
    #elif defined(_54_)
void PSNIP_TRAP()
{
    __asm__ __volatile__("ESTOP");
}
    #elif defined(_55_)
void PSNIP_TRAP()
{
    __asm__ __volatile__(";\n .if (.MNEMONIC)\n ESTOP_1\n .else\n ESTOP_1()\n .endif\n NOP");
}
    #elif defined(_64P_)
void PSNIP_TRAP()
{
    __asm__ __volatile__("SWBP 0");
}
    #elif defined(_6x_)
void PSNIP_TRAP()
{
    __asm__ __volatile__("NOP\n .word 0x10000000");
}
    #elif defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0) && defined(__GNUC__)
        #define PSNIP_TRAP() __builtin_trap()
    #else
        #include <signal.h>
        #if defined(SIGTRAP)
            #define PSNIP_TRAP() raise(SIGTRAP)
        #else
            #define PSNIP_TRAP() raise(SIGABRT)
        #endif
    #endif
#endif

#ifdef NDEBUG
    #define LUD_ASSERT_TRAP(x) Lud::assert::that(x);
#else
    #define LUD_ASSERT_TRAP(x) \
        if (!(x)) [[likely]]   \
        {                      \
            PSNIP_TRAP();      \
        }
#endif

namespace Lud::_detail_ {

template <typename... Args>
inline void log_and_die(const std::source_location loc, std::string_view msg, std::string_view fmt, Args&&... args)
{
    std::println(std::cerr, "[ASSERT FAIL] : {}({}:{})", loc.file_name(), loc.line(), loc.column());
    std::println(std::cerr, "        [WHY] : {}", std::vformat(fmt, std::make_format_args(args...)));
    if (!msg.empty())
    {
        std::println(std::cerr, "    [MESSAGE] : {}", msg);
    }
    std::exit(1);
}

} // namespace Lud::_detail_

namespace Lud::assert {
inline void that(bool expr, const std::string_view msg, const std::source_location loc)
{
    if (expr) [[likely]]
    {
        return;
    }

    _detail_::log_and_die(loc, msg, "expression was false");
}

template <class T1, class T2>
    requires std::equality_comparable_with<T1, T2>
void eq(T1 n1, T2 n2, const std::string_view msg, const std::source_location loc)
{
    if (n1 == n2) [[likely]]
    {
        return;
    }

    _detail_::log_and_die(loc, msg, "{} ≠ {}", n1, n2);
}

template <class T1, class T2>
    requires std::equality_comparable_with<T1, T2>
void ne(T1 n1, T2 n2, const std::string_view msg, const std::source_location loc)
{
    if (n1 != n2) [[likely]]
    {
        return;
    }

    _detail_::log_and_die(loc, msg, "{} = {}", n1, n2);
}

template <class T1, class T2>
    requires std::totally_ordered_with<T1, T2>
void gt(T1 n1, T2 n2, const std::string_view msg, const std::source_location loc)
{
    if (n1 > n2) [[likely]]
    {
        return;
    }

    _detail_::log_and_die(loc, msg, "{} ≤ {}", n1, n2);
}
template <class T1, class T2>
    requires std::totally_ordered_with<T1, T2>
void lt(T1 n1, T2 n2, const std::string_view msg, const std::source_location loc)
{
    if (n1 < n2) [[likely]]
    {
        return;
    }

    _detail_::log_and_die(loc, msg, "{} ≥ {}", n1, n2);
}

template <class T1, class T2>
    requires std::totally_ordered_with<T1, T2>
void geq(T1 n1, T2 n2, const std::string_view msg, const std::source_location loc)
{
    if (n1 >= n2) [[likely]]
    {
        return;
    }

    _detail_::log_and_die(loc, msg, "{} < {}", n1, n2);
}

template <class T1, class T2>
    requires std::totally_ordered_with<T1, T2>
void leq(T1 n1, T2 n2, const std::string_view msg, const std::source_location loc)
{
    if (n1 <= n2) [[likely]]
    {
        return;
    }

    _detail_::log_and_die(loc, msg, "{} > {} ", n1, n2);
}

template <class Min, class T, class Max>
    requires std::totally_ordered_with<Min, T> &&
             std::totally_ordered_with<Max, T>
void range(Min min, T val, Max max, const std::string_view msg, const std::source_location loc)
{
    if (min <= val && val < max) [[likely]]
    {
        return;
    }

    _detail_::log_and_die(loc, msg, "{} is not in the range [{}, {})", val, min, max);
}

} // namespace Lud::assert

namespace Lud::check {

inline void that(bool expr, const std::string_view msg)
{
    if (expr) [[likely]]
    {
        return;
    }

    throw std::runtime_error(std::string(msg));
}
inline void is_false(bool expr, const std::string_view msg)
{
    that(!expr, msg);
}
template <class T1, class T2>
    requires std::equality_comparable_with<T1, T2>
void in(T1 n1, std::initializer_list<T2> list, const std::string_view msg)
{
    for (const auto& elem : list)
    {
        if (n1 == elem)
        {
            return;
        }
    }
    throw std::runtime_error(std::string(msg));
}

template <class T1, class T2>
    requires std::equality_comparable_with<T1, T2>
inline void eq(T1 n1, T2 n2, const std::string_view msg)
{
    if (n1 == n2)
    {
        return;
    }

    throw std::runtime_error(std::format("{}: [{} != {}]", msg, n1, n2));
}

template <class T1, class T2>
    requires std::equality_comparable_with<T1, T2>
inline void ne(T1 n1, T2 n2, const std::string_view msg)
{
    if (n1 != n2) [[likely]]
    {
        return;
    }

    throw std::runtime_error(std::format("{}: [{} == {}]", msg, n1, n2));
}

template <class T1, class T2>
    requires std::totally_ordered_with<T1, T2>
inline void gt(T1 n1, T2 n2, const std::string_view msg)
{
    if (n1 > n2) [[likely]]
    {
        return;
    }

    throw std::runtime_error(std::format("{}: [{} <= {}]", msg, n1, n2));
}

template <class T1, class T2>
    requires std::totally_ordered_with<T1, T2>
inline void lt(T1 n1, T2 n2, const std::string_view msg)
{
    if (n1 < n2) [[likely]]
    {
        return;
    }

    throw std::runtime_error(std::format("{}: [{} >= {}]", msg, n1, n2));
}

template <class T1, class T2>
    requires std::totally_ordered_with<T1, T2>
inline void geq(T1 n1, T2 n2, const std::string_view msg)
{
    if (n1 >= n2) [[likely]]
    {
        return;
    }

    throw std::runtime_error(std::format("{}: [{} < {}]", msg, n1, n2));
}

template <class T1, class T2>
    requires std::totally_ordered_with<T1, T2>
inline void leq(T1 n1, T2 n2, const std::string_view msg)
{
    if (n1 <= n2) [[likely]]
    {
        return;
    }

    throw std::runtime_error(std::format("{}: [{} > {}]", msg, n1, n2));
}

template <class Min, class T, class Max>
    requires std::totally_ordered_with<Min, T> &&
             std::totally_ordered_with<Max, T>
inline void range(Min min, T val, Max max, const std::string_view msg)
{
    if (min <= val && val < max) [[likely]]
    {
        return;
    }

    throw std::out_of_range(std::string(msg));
}

} // namespace Lud::check
#endif // LUD_ASSERT_HEADER
