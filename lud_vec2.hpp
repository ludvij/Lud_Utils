#ifndef LUD_VEC2_HEADER
#define LUD_VEC2_HEADER
#include <compare>
#include <functional>
#include <ostream>

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

template<NumberType N>
struct Vec2 {
	Vec2(N x, N y);
	N x;
	N y;

	std::weak_ordering operator<=>(const Vec2& o) const = default;
	bool operator==(const Vec2& o) const = default;

	Vec2<N>& operator+(Vec2<N> lhs, const Vec2<N>& rhs);
	Vec2<N>& operator-(Vec2<N> lhs, const Vec2<N>& rhs);

};




// IMPLEMENTATION

template<NumberType N> 
inline std::ostream& operator<<(std::ostream& os, const Vec2<N>& v) 
{
	os << '[' << v.x << ", " << v.y << ']';
	return os;
}

template <NumberType N> struct std::hash<Vec2<N>> {
	size_t operator()(const Vec2<N>& s) const noexcept {
		const size_t x = std::hash<size_t>{}(s.x);
		const size_t y = std::hash<size_t>{}(s.y);

		return (x * 73856093) ^ y;
	}
};

template<NumberType N>
inline Vec2<N>::Vec2(N x, N y)
	: x(x)
	, y(y)
{

}

template <NumberType N>
inline Vec2<N>& Vec2<N>::operator-(Vec2<N> lhs, const Vec2<N> &rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;

	return lhs;
}



template<NumberType N>
inline Vec2<N>& Vec2<N>::operator+(Vec2<N> lhs, const Vec2<N>& rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;

	return lhs;
}

}

#endif