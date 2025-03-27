#ifndef LUD_PARSE_HEADER
#define LUD_PARSE_HEADER
#include <charconv>
#include <optional>
#include <string_view>

namespace Lud
{



#ifndef LUD_NUMBER_TYPE_DEFINED
#define LUD_NUMBER_TYPE_DEFINED
template<typename T>
concept NumberType = requires( T param )
{
	requires std::is_arithmetic_v<T>;
	requires !std::is_same_v<bool, T>;
	requires !std::is_pointer_v<T>;
};
template<typename T>
concept IntegerType = requires( T param )
{
	requires std::is_integral_v<T>;
	requires !std::is_pointer_v<T>;
	requires !std::is_same_v<bool, T>;
};

template<typename T>
concept RealType = requires( T param )
{
	requires std::is_floating_point_v<T>;
	requires !std::is_pointer_v<T>;
	requires !std::is_same_v<bool, T>;
};
#endif//LUD_NUMBER_TYPE_DEFINED

template<IntegerType N> N parse_num(std::string_view sv, int base=10);
template<IntegerType N> std::optional<N> is_num(std::string_view sv, int base=10);

template<RealType N> N parse_num(std::string_view sv, std::chars_format fmt=std::chars_format::general);
template<RealType N> std::optional<N> is_num(std::string_view sv, std::chars_format fmt=std::chars_format::general);

}

template<Lud::IntegerType N>
inline N Lud::parse_num(const std::string_view sv, int base/*=10*/)
{
	N val{};
	std::from_chars(sv.data(), sv.data() + sv.size(), val, base);
	return val;
}

template<Lud::IntegerType N>
inline std::optional<N> Lud::is_num(const std::string_view sv, int base/*=10*/)
{
	N val{};
	if (std::from_chars(sv.data(), sv.data() + sv.size(), val, base).ec == std::errc::invalid_argument)
	{
		return std::nullopt;
	}
	return val;
}


template<Lud::RealType N>
inline N Lud::parse_num(const std::string_view sv, const std::chars_format fmt/*=std::chars_format::general)*/)
{
	N val{};
	std::from_chars(sv.data(), sv.data() + sv.size(), val, fmt);
	return val;
}

template<Lud::RealType N>
inline std::optional<N> Lud::is_num(const std::string_view sv, const std::chars_format fmt/*=std::chars_format::general)*/)
{
	N val{};
	if (std::from_chars(sv.data(), sv.data() + sv.size(), val, fmt).ec == std::errc::invalid_argument)
	{
		return std::nullopt;
	}
	return val;
}


#endif//!LUD_PARSE_HEADER