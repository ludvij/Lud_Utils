#ifndef LUD_ASSERT_HEADER
#define LUD_ASSERT_HEADER

#include <cstdlib>
#include <iostream>
#include <source_location>
#include <sstream>
#include <functional>

namespace Lud::assert {

// hacky workaround when there are a lot of prints
#define NO_PRINT(code) \
	std::streambuf *lud_assert_old_cout = std::cout.rdbuf(); std::stringstream lud_assert_ss_cout; std::cout.rdbuf(lud_assert_ss_cout.rdbuf()); \
	std::streambuf *lud_assert_old_cerr = std::cerr.rdbuf(); std::stringstream lud_assert_ss_cerr; std::cerr.rdbuf(lud_assert_ss_cerr.rdbuf()); \
	std::streambuf *lud_assert_old_clog = std::clog.rdbuf(); std::stringstream lud_assert_ss_clog; std::clog.rdbuf(lud_assert_ss_clog.rdbuf()); \
	code \
	std::cout.rdbuf(lud_assert_old_cout); \
	std::cerr.rdbuf(lud_assert_old_cerr); \
	std::cerr.rdbuf(lud_assert_old_clog); \

#define assert_no_print(assert_type, v1, v2) NO_PRINT(Lud::assert::##assert_type(v2, v2);)


#if !defined(NDEBUG)

                             void eq(bool expr, const std::source_location loc = std::source_location::current());
template<class T1, class T2> void eq(T1 n1, T2 n2, const std::source_location loc = std::source_location::current());
template<class T1, class T2> void ne(T1 n1, T2 n2, const std::source_location loc = std::source_location::current());
template<class T1, class T2> void greater(T1 n1, T2 n2, const std::source_location loc = std::source_location::current());
template<class T1, class T2> void lower(T1 n1, T2 n2, const std::source_location loc = std::source_location::current());
template<class T1, class T2> void geq(T1 n1, T2 n2, const std::source_location loc = std::source_location::current());
template<class T1, class T2> void leq(T1 n1, T2 n2, const std::source_location loc = std::source_location::current());


template<typename... Args> void log_assertion_fail(const std::source_location loc, Args... args);

void eq(bool expr, const std::source_location loc)
{
	if (expr) return;

	log_assertion_fail(loc, "The expression evaluated to false");
	std::exit(EXIT_FAILURE);
}



template<class T1, class T2> void eq(T1 n1, T2 n2, const std::source_location loc)
{
	if (n1 == n2) return;

	log_assertion_fail(loc, n1, " != ", n2);
	std::exit(EXIT_FAILURE);
}

template<class T1, class T2> void ne(T1 n1, T2 n2, const std::source_location loc)
{
	if (n1 != n2) return;

	log_assertion_fail(loc, n1, " == ", n2);
	std::exit(EXIT_FAILURE);
}

template<class T1, class T2> void greater(T1 n1, T2 n2, const std::source_location loc)
{
	if (n1 > n2) return;

	log_assertion_fail(loc, n1, " is not greater than ", n2 );
	std::exit(EXIT_FAILURE);
}

template<class T1, class T2> void lower(T1 n1, T2 n2, const std::source_location loc)
{
	if (n1 < n2) return;

	log_assertion_fail(loc, n1, " is not lower than ", n2);
	std::exit(EXIT_FAILURE);
}

template<class T1, class T2> void geq(T1 n1, T2 n2, const std::source_location loc)
{
	if (n1 >= n2) return;

	log_assertion_fail(loc, n1, " is not greater or equal to ", n2);
	std::exit(EXIT_FAILURE);
}

template<class T1, class T2> void leq(T1 n1, T2 n2, const std::source_location loc)
{
	if (n1 <= n2) return;

	log_assertion_fail(loc, n1, " is not lower or equal to ", n2);
	std::exit(EXIT_FAILURE);
}

template<typename... Args> void log_assertion_fail(const std::source_location loc, Args... args)
{
	std::clog << "[ASSERT FAIL] " << loc.file_name() << '(' << loc.line() << ':' << loc.column() << ")\n";
	std::clog << "              ";
	(std::clog << ... << args) << '\n';
}


#else
                             void eq(bool expr) {}
template<class T1, class T2> void eq(T1 n1, T2 n2) {}
template<class T1, class T2> void ne(T1 n1, T2 n2) {}
template<class T1, class T2> void greater(T1 n1, T2 n2) {}
template<class T1, class T2> void lower(T1 n1, T2 n2) {}
template<class T1, class T2> void geq(T1 n1, T2 n2) {}
template<class T1, class T2> void leq(T1 n1, T2 n2) {}

#endif
}
#endif//LUD_ASSERT_HEADER