#ifndef LUD_ASSERT_HEADER
#define LUD_ASSERT_HEADER


#include <cstdio>
#include <cstdlib>
#include <fcntl.h>

#include <format>
#include <functional>
#include <iostream>
#include <source_location>
#include <sstream>
#include <string_view>


// hacky workaround when there are a lot of prints

#define CAPTURE_STDOUT(code) \
do {\
	fflush(stdout); \
	int stdout_fd = dup(fileno(stdout)); \
	int redir_fd = open("redirected_stdout", O_WRONLY); \
	dup2(redir_fd, fileno(stdout)); \
	close(redir_fd); \
	code; \
	fflush(stdout); \
	dup2(stdout_fd, fileno(stdout)); \
	close(stdout_fd); \
} while (0)
#define CAPTURE_COUT(code) \
do { \
	std::streambuf *lud_assert_old_cout = std::cout.rdbuf(); std::stringstream lud_assert_ss_cout; std::cout.rdbuf(lud_assert_ss_cout.rdbuf()); \
	std::streambuf *lud_assert_old_cerr = std::cerr.rdbuf(); std::stringstream lud_assert_ss_cerr; std::cerr.rdbuf(lud_assert_ss_cerr.rdbuf()); \
	std::streambuf *lud_assert_old_clog = std::clog.rdbuf(); std::stringstream lud_assert_ss_clog; std::clog.rdbuf(lud_assert_ss_clog.rdbuf()); \
	code; \
	std::cout.rdbuf(lud_assert_old_cout); \
	std::cerr.rdbuf(lud_assert_old_cerr); \
	std::cerr.rdbuf(lud_assert_old_clog); \
} while (0) 
#define NO_PRINT(code) CAPTURE_STDOUT(code)


#define assert_no_print(assert_type, ...) NO_PRINT(Lud::assert::##assert_type(__VA_ARGS__))
#define check_no_print(check_type, ...) NO_PRINT(Lud::check::##check_type(__VA_ARGS__))



#if !defined(NDEBUG) 

namespace Lud::assert
{

inline void eq(bool expr, const std::string_view msg= "", const std::source_location loc = std::source_location::current());
inline void that(bool expr, const std::string_view msg= "", const std::source_location loc = std::source_location::current());
template<class T1, class T2> void eq(T1 n1, T2 n2, const std::string_view msg= "", const std::source_location loc = std::source_location::current());
template<class T1, class T2> void ne(T1 n1, T2 n2, const std::string_view msg= "", const std::source_location loc = std::source_location::current());
template<class T1, class T2> void gt(T1 n1, T2 n2, const std::string_view msg= "", const std::source_location loc = std::source_location::current());
template<class T1, class T2> void lt(T1 n1, T2 n2, const std::string_view msg= "", const std::source_location loc = std::source_location::current());
template<class T1, class T2> void geq(T1 n1, T2 n2, const std::string_view msg= "", const std::source_location loc = std::source_location::current());
template<class T1, class T2> void leq(T1 n1, T2 n2, const std::string_view msg= "", const std::source_location loc = std::source_location::current());
template<class Min, class T, class Max> void range(Min min, T val, Max max, const std::string_view msg= "", const std::source_location loc = std::source_location::current());

}
#endif

namespace Lud::check
{

inline void eq(bool expr, const std::string_view msg = "The expression evaluated to false");
template<class T1, class T2> void eq(T1 n1, T2 n2, const std::string_view msg = "passed arguments are not the same");
template<class T1, class T2> void ne(T1 n1, T2 n2, const std::string_view msg = "passed arguments are the same");
template<class T1, class T2> void gt(T1 n1, T2 n2, const std::string_view msg = "n1 is not greater than n2");
template<class T1, class T2> void lt(T1 n1, T2 n2, const std::string_view msg = "n1 is not lower than n2");
template<class T1, class T2> void geq(T1 n1, T2 n2, const std::string_view msg = "n1 is lower than n2");
template<class T1, class T2> void leq(T1 n1, T2 n2, const std::string_view msg = "n1 is greater than n2");
template<class Min, class T, class Max> void range(Min min, T val, Max max, const std::string_view msg = "val is not in range [min, max)");

}

#if !defined(NDEBUG) 

// https://github.com/nemequ/portable-snippets/blob/master/debug-trap/debug-trap.h

#if defined(__has_builtin) && !defined(__ibmxl__)
#  if __has_builtin(__builtin_debugtrap)
#    define psnip_trap() __builtin_debugtrap()
#  elif __has_builtin(__debugbreak)
#    define psnip_trap() __debugbreak()
#  endif
#endif
#if !defined(psnip_trap)
#  if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#    define psnip_trap() __debugbreak()
#  elif defined(__ARMCC_VERSION)
#    define psnip_trap() __breakpoint(42)
#  elif defined(__ibmxl__) || defined(__xlC__)
#    include <builtins.h>
#    define psnip_trap() __trap(42)
#  elif defined(__DMC__) && defined(_M_IX86)
static inline void psnip_trap(void)
{
	__asm int 3h;
}
#  elif defined(__i386__) || defined(__x86_64__)
static inline void psnip_trap(void)
{
	__asm__ __volatile__("int3");
}
#  elif defined(__thumb__)
static inline void psnip_trap(void)
{
	__asm__ __volatile__(".inst 0xde01");
}
#  elif defined(__aarch64__)
static inline void psnip_trap(void)
{
	__asm__ __volatile__(".inst 0xd4200000");
}
#  elif defined(__arm__)
static inline void psnip_trap(void)
{
	__asm__ __volatile__(".inst 0xe7f001f0");
}
#  elif defined (__alpha__) && !defined(__osf__)
static inline void psnip_trap(void)
{
	__asm__ __volatile__("bpt");
}
#  elif defined(_54_)
static inline void psnip_trap(void)
{
	__asm__ __volatile__("ESTOP");
}
#  elif defined(_55_)
static inline void psnip_trap(void)
{
	__asm__ __volatile__(";\n .if (.MNEMONIC)\n ESTOP_1\n .else\n ESTOP_1()\n .endif\n NOP");
}
#  elif defined(_64P_)
static inline void psnip_trap(void)
{
	__asm__ __volatile__("SWBP 0");
}
#  elif defined(_6x_)
static inline void psnip_trap(void)
{
	__asm__ __volatile__("NOP\n .word 0x10000000");
}
#  elif defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0) && defined(__GNUC__)
#    define psnip_trap() __builtin_trap()
#  else
#    include <signal.h>
#    if defined(SIGTRAP)
#      define psnip_trap() raise(SIGTRAP)
#    else
#      define psnip_trap() raise(SIGABRT)
#    endif
#  endif
#endif


#define LUD_ASSERT_TRAP() psnip_trap()


namespace Lud::Detail
{


template <typename... Args>
inline void log_fail(const std::source_location loc, std::string_view name, std::string_view msg, Args&&... args)
{
	std::cout << "[ASSERT FAIL] " << loc.file_name() << "(" << loc.line() << ", " << loc.column() << ")\n";
	std::cout << std::vformat("{: >13s} → ", std::make_format_args(name)) << std::vformat(msg, std::make_format_args(args...)) << '\n';
}

}

namespace Lud::assert
{
inline void that(bool expr, const std::string_view msg, const std::source_location loc)
{
	if (expr) return;

	if (msg.empty())
		Detail::log_fail(loc, "ASSERTION", "The expression evaluated to false");
	else
		Detail::log_fail(loc, "ASSERTION", msg);

	LUD_ASSERT_TRAP();
}
inline void eq(bool expr, const std::string_view msg, const std::source_location loc)
{
	if (expr) return;

	if (msg.empty())
		Detail::log_fail(loc, "EQUAL", "The expression evaluated to false");
	else
		Detail::log_fail(loc, "EQUAL", msg);

	LUD_ASSERT_TRAP();
}

template<class T1, class T2>
inline void eq(T1 n1, T2 n2, const std::string_view msg, const std::source_location loc)
{
	if (n1 == n2) return;

	if (msg.empty())
		Detail::log_fail(loc, "EQUAL", "{} ≠ {}", n1, n2);
	else
		Detail::log_fail(loc, "EQUAL", msg);

	LUD_ASSERT_TRAP();

}

template<class T1, class T2>
inline void ne(T1 n1, T2 n2, const std::string_view msg, const std::source_location loc)
{
	if (n1 != n2) return;

	if (msg.empty())
		Detail::log_fail(loc, "DIFFERENT", "{} = {}", n1, n2);
	else
		Detail::log_fail(loc, "DIFFERENT", msg);

	LUD_ASSERT_TRAP();
}

template<class T1, class T2>
inline void gt(T1 n1, T2 n2, const std::string_view msg, const std::source_location loc)
{
	if (n1 > n2) return;

	if (msg.empty())
		Detail::log_fail(loc, "GREATER THAN", "{} ≤ {}", n1, n2);
	else
		Detail::log_fail(loc, "GREATER THAN", msg);

	LUD_ASSERT_TRAP();
}

template<class T1, class T2>
inline void lt(T1 n1, T2 n2, const std::string_view msg, const std::source_location loc)
{
	if (n1 < n2) return;

	if (msg.empty())
		Detail::log_fail(loc, "LOWER THAN", "{} ≥ {}", n1, n2);
	else
		Detail::log_fail(loc, "LOWER THAN", msg);

	LUD_ASSERT_TRAP();
}

template<class T1, class T2>
inline void geq(T1 n1, T2 n2, const std::string_view msg, const std::source_location loc)
{
	if (n1 >= n2) return;

	if (msg.empty())
		Detail::log_fail(loc, "GREATER EQUAL", "{} < {}", n1, n2);
	else
		Detail::log_fail(loc, "GREATER EQUAL", msg);

	LUD_ASSERT_TRAP();
}

template<class T1, class T2>
inline void leq(T1 n1, T2 n2, const std::string_view msg, const std::source_location loc)
{
	if (n1 <= n2) return;
	if (msg.empty())
		Detail::log_fail(loc, "LOWER EQUAL", "{} > {} ", n1, n2);
	else
		Detail::log_fail(loc, "LOWER EQUAL", msg);

	LUD_ASSERT_TRAP();
}

template<class Min, class T, class Max>
inline void range(Min min, T val, Max max, const std::string_view msg, const std::source_location loc)
{
	if (min <= val && val < max) return;
	if (msg.empty())
		Detail::log_fail(loc, "RANGE", "{} is not in the range [{}, {})", val, min, max);
	else
		Detail::log_fail(loc, "RANGE", msg);

	LUD_ASSERT_TRAP();
}

}



#else
#define assert_trap(assert_type, ...) 

namespace Lud::assert
{
constexpr inline void eq(bool expr, const std::string_view msg="")
{
	return;
}
template<class T1, class T2>
constexpr inline void eq(T1 n1, T2 n2, const std::string_view msg="")
{
	return;
}
template<class T1, class T2>
constexpr inline void ne(T1 n1, T2 n2, const std::string_view msg="")
{
	return;
}
template<class T1, class T2>
constexpr inline void gt(T1 n1, T2 n2, const std::string_view msg="")
{
	return;
}
template<class T1, class T2>
constexpr inline void lt(T1 n1, T2 n2, const std::string_view msg="")
{
	return;
}
template<class T1, class T2>
constexpr inline void geq(T1 n1, T2 n2, const std::string_view msg="")
{
	return;
}
template<class T1, class T2>
constexpr inline void leq(T1 n1, T2 n2, const std::string_view msg="")
{
	return;
}
template<class Min, class T, class Max>
constexpr inline void range(Min min, T val, Max max, const std::string_view msg="")
{
	return;
}
}


#endif
namespace Lud::check
{

inline void eq(bool expr, const std::string_view msg)
{
	if (expr) return;

	throw std::logic_error(std::string(msg));
}
template<class T1, class T2>
inline void eq(T1 n1, T2 n2, const std::string_view msg)
{
	if (n1 == n2) return;

	throw std::logic_error(std::string(msg));
}


template<class T1, class T2>
inline void ne(T1 n1, T2 n2, const std::string_view msg)
{
	if (n1 != n2) return;

	throw std::logic_error(std::string(msg));

}

template<class T1, class T2>
inline void gt(T1 n1, T2 n2, const std::string_view msg)
{
	if (n1 > n2) return;

	throw std::logic_error(std::string(msg));
}

template<class T1, class T2>
inline void lt(T1 n1, T2 n2, const std::string_view msg)
{
	if (n1 < n2) return;

	throw std::logic_error(std::string(msg));
}

template<class T1, class T2>
inline void geq(T1 n1, T2 n2, const std::string_view msg)
{
	if (n1 >= n2) return;

	throw std::logic_error(std::string(msg));
}

template<class T1, class T2>
inline void leq(T1 n1, T2 n2, const std::string_view msg)
{
	if (n1 <= n2) return;

	throw std::logic_error(std::string(msg));
}

template<class Min, class T, class Max>
inline void range(Min min, T val, Max max, const std::string_view msg)
{
	if (min <= val && val < max) return;

	throw std::logic_error(std::string(msg));
}

}

#endif//LUD_ASSERT_HEADER