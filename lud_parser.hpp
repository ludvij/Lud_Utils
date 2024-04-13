#ifndef LUD_PARSER_HEADER
#define LUD_PARSER_HEADER
#include <charconv>
#include <string_view>


namespace Lud {

#ifndef LUD_NUMBER_TYPE_DEFINED
#define LUD_NUMBER_TYPE_DEFINED
template<typename T>
concept NumberType = requires(T param)
{
    requires std::is_integral_v<T> || std::is_floating_point_v<T>;
    requires !std::is_same_v<bool, T>;
    requires std::is_arithmetic_v<decltype(param +1)>;
    requires !std::is_pointer_v<T>;
};
#endif

template<NumberType N> N parse_num(std::string_view sv);



template<NumberType N> N parse_num(std::string_view sv)
{
	N val = 0;
	std::from_chars(sv.data(), sv.data() + sv.size(), val);
	return val;
}

}
#endif