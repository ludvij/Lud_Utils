#ifndef LUD_PARSE_HEADER
#define LUD_PARSE_HEADER

#include <algorithm>
#include <charconv>
#include <iterator>
#include <limits>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace Lud {

template <typename TypeT>
concept NumberType = requires {
    requires std::is_arithmetic_v<TypeT>;
    requires !std::is_pointer_v<TypeT>;
    requires !std::same_as<bool, TypeT>;
};
template <typename TypeT>
concept IntegerType = requires {
    requires std::is_integral_v<TypeT>;
    requires !std::is_pointer_v<TypeT>;
    requires !std::same_as<bool, TypeT>;
};

template <typename T>
concept RealType = requires {
    requires std::is_floating_point_v<T>;
    requires !std::is_pointer_v<T>;
    requires !std::same_as<bool, T>;
};

template <typename RangeT>
concept string_container = requires(RangeT rng) {
    requires std::ranges::range<RangeT>;
    requires std::convertible_to<std::ranges::range_value_t<RangeT>, std::string_view>;
};

template <typename RangeT>
concept range_of_string = requires(RangeT r) {
    requires std::ranges::range<RangeT>;
    requires std::same_as<std::ranges::range_value_t<RangeT>, std::string>;
};

template <typename RangeT>
concept range_of_string_view = requires(RangeT r) {
    requires std::ranges::range<RangeT>;
    requires std::same_as<std::ranges::range_value_t<RangeT>, std::string_view>;
};

template <IntegerType N>
std::optional<N> is_num(const std::string_view sv, int base = 10);
template <RealType N>
std::optional<N> is_num(const std::string_view sv, std::chars_format fmt = std::chars_format::general);

template <RealType N>
std::optional<N> is_fraction(const std::string_view sv);
template <RealType N>
std::optional<N> is_percentage(const std::string_view sv);

template <range_of_string_view R = std::vector<std::string_view>>
R Split(const std::string_view str, const std::string_view delim, size_t n = 0);

template <range_of_string_view R = std::vector<std::string_view>>
R Split(const std::string_view str, char delim, size_t n = 0);

template <string_container Container>
std::string Join(const Container& container, const std::string_view delim);

template <string_container Container>
std::string Join(const Container& container, char delim);

template <std::input_iterator T>
std::string Join(T first, T last, const std::string_view delim);

template <std::input_iterator T>
std::string Join(T first, T last, char delim);

std::string RemovePrefix(const std::string_view str, const std::string_view prefix);

std::string RemoveSuffix(const std::string_view str, const std::string_view suffix);

std::string ToUpper(const std::string_view str);

std::string ToLower(const std::string_view str);

std::string ToTitle(const std::string_view str);

std::string Capitalize(const std::string_view str);

std::string LStrip(const std::string_view str);

std::string RStrip(const std::string_view str);

std::string Strip(const std::string_view str);

std::string Reverse(const std::string_view str);

std::string Replace(const std::string_view str, const std::string_view pattern, const std::string_view replacement);

std::string Replace(const std::string_view str, char pattern, char replacement);

bool ContainsAny(const std::string_view str, const std::string_view pattern);

bool ContainsAll(const std::string_view str, const std::string_view pattern);

namespace inplace {
std::string& RemovePrefix(std::string& str, const std::string_view prefix);

std::string& RemoveSuffix(std::string& str, const std::string_view suffix);

std::string& ToUpper(std::string& str);

std::string& ToLower(std::string& str);

std::string& ToTitle(std::string& str);

std::string& Capitalize(std::string& str);

std::string& LStrip(std::string& str);

std::string& RStrip(std::string& str);

std::string& Strip(std::string& str);

std::string& Reverse(std::string& str);

std::string& Replace(std::string& str, const std::string_view pattern, const std::string_view replacement);

std::string& Replace(std::string& str, char pattern, char replacement);
} // namespace inplace

} // namespace Lud

// implementation==============================================================================

template <Lud::IntegerType N>
std::optional<N> Lud::is_num(const std::string_view sv, int base /*=10*/)
{
    // house keeping since from chars does not recognize leading plus sign and leading whitespace
    // auto check = ToUpper(Strip(sv));
    // inplace::RemovePrefix(check, "+");
    // switch (base)
    // {
    // case 16:
    //     inplace::RemovePrefix(check, "0X");
    //     break;
    // case 2:
    //     inplace::RemovePrefix(check, "0B");
    //     break;
    // case 8:
    //     inplace::RemovePrefix(check, "0O");
    //     inplace::RemovePrefix(check, "0");
    //     break;
    // default:
    //     break;
    // }

    N val{};
    const auto first = sv.data();
    const auto last = first + sv.size();
    const auto res = std::from_chars(first, last, val, base);
    if (res.ec == std::errc::invalid_argument || res.ec == std::errc::result_out_of_range)
    {
        return std::nullopt;
    }
    if (res.ptr != last)
    {
        return std::nullopt;
    }
    return val;
}

template <Lud::RealType N>
std::optional<N> Lud::is_num(const std::string_view sv, const std::chars_format fmt /*=std::chars_format::general)*/)
{
    // auto check = ToUpper(Strip(sv));
    // if (std::to_underlying(fmt) & std::to_underlying(std::chars_format::hex))
    // {
    //     inplace::RemovePrefix(check, "0X");
    // }
    // else
    // {
    //     inplace::RemovePrefix(check, "+");
    // }

    N val{};
    const auto first = sv.data();
    const auto last = first + sv.size();
    const auto res = std::from_chars(first, last, val, fmt);
    if (res.ec == std::errc::invalid_argument || res.ec == std::errc::result_out_of_range)
    {
        return std::nullopt;
    }
    if (res.ptr != last)
    {
        return std::nullopt;
    }
    return val;
}

template <Lud::RealType N>
std::optional<N> Lud::is_fraction(const std::string_view sv)
{
    if (!sv.contains("/"))
    {
        return std::nullopt;
    }
    auto parts = Lud::Split(sv, "/");
    if (parts.size() != 2)
    {
        return std::nullopt;
    }

    auto numerator = Lud::is_num<N>(parts[0]);
    auto denominator = Lud::is_num<N>(parts[1]);

    if (!(numerator && denominator))
    {
        return std::nullopt;
    }
    if (denominator.value() == 0)
    {
        return std::numeric_limits<N>::quiet_NaN();
    }
    return numerator.value() / denominator.value();
}

template <Lud::RealType N>
std::optional<N> Lud::is_percentage(const std::string_view sv)
{
    auto check = Lud::Strip(sv);
    const auto first = check.find_first_of('%');
    if (!(first != std::string::npos && first == check.find_last_of('%')))
    {
        return std::nullopt;
    }

    Lud::inplace::RemoveSuffix(check, "%");

    auto num = Lud::is_num<N>(check);
    if (!num)
    {
        return std::nullopt;
    }

    return num.value() / 100.0f;
}

template <Lud::string_container Container>
std::string Lud::Join(const Container& container, const std::string_view delim)
{
    return Join(container.begin(), container.end(), delim);
}

template <Lud::string_container Container>
std::string Lud::Join(const Container& container, char delim)
{
    return Join(container.begin(), container.end(), delim);
}

template <std::input_iterator T>
std::string Lud::Join(T first, T last, const std::string_view delim)
{
    std::string res;
    if (first >= last)
    {
        return res;
    }
    res.append(static_cast<std::string>(*first));
    for (auto it = first + 1; it != last; ++it)
    {
        res.append(delim);
        res.append(static_cast<std::string>(*it));
    }

    return res;
}

template <std::input_iterator T>
std::string Lud::Join(T first, T last, char delim)
{
    std::string res;
    if (first >= last)
    {
        return res;
    }
    res.append(static_cast<std::string>(*first));
    for (auto it = first + 1; it != last; ++it)
    {
        res += delim;
        res.append(static_cast<std::string>(*it));
    }

    return res;
}

inline std::string Lud::RemovePrefix(const std::string_view str, const std::string_view prefix)
{

    if (str.starts_with(prefix))
    {
        return std::string(str.substr(prefix.size()));
    }

    return std::string(str);
}

inline std::string Lud::RemoveSuffix(const std::string_view str, const std::string_view suffix)
{
    if (str.ends_with(suffix))
    {
        return std::string(str.substr(0, str.size() - suffix.size()));
    }

    return std::string(str);
}

inline std::string& Lud::inplace::RemovePrefix(std::string& str, const std::string_view prefix)
{
    if (str.starts_with(prefix))
    {
        str.erase(0, prefix.size());
    }
    return str;
}

inline std::string& Lud::inplace::RemoveSuffix(std::string& str, const std::string_view suffix)
{
    if (str.ends_with(suffix))
    {
        str.erase(str.size() - suffix.size());
    }
    return str;
}

template <Lud::range_of_string_view R>
R Lud::Split(const std::string_view str, const std::string_view delim, size_t n)
{
    R r;
    if (delim.empty())
    {
        std::inserter(r, r.end()) = str;
        return r;
    }
    size_t next = str.find(delim);
    std::string_view inner_str = str;
    while (next != std::string_view::npos && (n != std::ranges::size(r) || n == 0))
    {
        // skips empty
        if (next != 0)
        {
            std::inserter(r, r.end()) = inner_str.substr(0, next);
        }
        inner_str.remove_prefix(next + delim.size());
        next = inner_str.find(delim);
    }
    if (!inner_str.empty())
    {
        std::inserter(r, r.end()) = inner_str;
    }

    return r;
}
template <Lud::range_of_string_view R>
R Lud::Split(const std::string_view str, char delim, size_t n)
{
    R r;
    size_t next = str.find(delim);
    std::string_view inner_str = str;
    while (next != std::string_view::npos && (n != std::ranges::size(r) || n == 0))
    {
        // skips empty
        if (next != 0)
        {
            std::inserter(r, r.end()) = inner_str.substr(0, next);
        }
        inner_str.remove_prefix(next + 1);
        next = inner_str.find(delim);
    }
    if (!inner_str.empty())
    {
        std::inserter(r, r.end()) = inner_str;
    }

    return r;
}

inline std::string Lud::ToUpper(const std::string_view str)
{
    std::string res(str);

    inplace::ToUpper(res);

    return res;
}

inline std::string Lud::ToLower(const std::string_view str)
{
    std::string res(str);

    inplace::ToLower(res);

    return res;
}

inline std::string Lud::ToTitle(const std::string_view str)
{
    std::string res(str);

    inplace::ToTitle(res);

    return res;
}

inline std::string Lud::Capitalize(const std::string_view str)
{
    std::string res(str);

    inplace::Capitalize(res);

    return res;
}

inline std::string Lud::LStrip(const std::string_view str)
{
    const auto delims = "\t\n\r ";
    const auto idx = str.find_first_not_of(delims);

    if (idx == std::string::npos)
    {
        return {};
    }

    return std::string(str.substr(idx));
}

inline std::string Lud::RStrip(const std::string_view str)
{
    const auto delims = "\t\n\r ";
    const auto idx = str.find_last_not_of(delims);

    if (idx == std::string::npos)
    {
        return {};
    }

    return std::string(str.substr(0, idx + 1));
}

inline std::string Lud::Strip(const std::string_view str)
{
    std::string res(str);

    inplace::Strip(res);

    return res;
}

inline std::string Lud::Reverse(const std::string_view str)
{
    return {str.rbegin(), str.rend()};
}

inline std::string Lud::Replace(const std::string_view str, const std::string_view pattern, const std::string_view replacement)
{
    std::string res{str};
    return inplace::Replace(res, pattern, replacement);
}

inline std::string Lud::Replace(const std::string_view str, char pattern, char replacement)
{
    std::string res{str};
    return inplace::Replace(res, pattern, replacement);
}

inline bool Lud::ContainsAny(const std::string_view str, const std::string_view pattern)
{
    for (const auto& c : str)
    {
        if (pattern.contains(c))
        {
            return true;
        }
    }
    return false;
}

inline bool Lud::ContainsAll(const std::string_view str, const std::string_view pattern)
{
    size_t counter = 0;
    for (const auto& c : str)
    {
        if (pattern.contains(c))
        {
            counter++;
        }
        if (counter >= pattern.size())
        {
            return true;
        }
    }
    return false;
}

inline std::string& Lud::inplace::ToUpper(std::string& str)
{
    std::ranges::transform(str, str.begin(), [](char c) {
        return std::toupper(c);
    });

    return str;
}

inline std::string& Lud::inplace::ToLower(std::string& str)
{
    std::ranges::transform(str, str.begin(), [](char c) {
        return std::tolower(c);
    });
    return str;
}

inline std::string& Lud::inplace::Capitalize(std::string& str)
{
    if (!str.empty())
    {
        str[0] = static_cast<char>(std::toupper(str[0]));
    }
    return str;
}

inline std::string& Lud::inplace::ToTitle(std::string& str)
{
    const auto delim = "\t\n\r ";
    auto idx = str.find_first_not_of(delim);
    while (idx != std::string::npos)
    {
        str[idx] = static_cast<char>(std::toupper(str[idx]));
        idx = str.find_first_of(delim, idx + 1);
        if (idx != std::string::npos)
        {
            idx = str.find_first_not_of(delim, idx + 1);
        }
    }
    return str;
}

inline std::string& Lud::inplace::LStrip(std::string& str)
{
    const auto delims = "\t\n\r ";
    const auto idx = str.find_first_not_of(delims);

    str.erase(0, idx);
    return str;
}

inline std::string& Lud::inplace::RStrip(std::string& str)
{
    const auto delims = "\t\n\r ";
    const auto idx = str.find_last_not_of(delims);

    str.erase(idx + 1);

    return str;
}

inline std::string& Lud::inplace::Strip(std::string& str)
{
    const auto delims = "\t\n\r ";
    const auto first = str.find_first_not_of(delims);
    const auto last = str.find_last_not_of(delims);

    str.erase(last + 1);
    str.erase(0, first);

    return str;
}

inline std::string& Lud::inplace::Reverse(std::string& str)
{
    std::ranges::reverse(str);
    return str;
}
inline std::string& Lud::inplace::Replace(std::string& str, const std::string_view pattern, const std::string_view replacement)
{
    size_t pos = str.find(pattern);
    while (pos != std::string::npos)
    {
        str.replace(pos, pattern.size(), replacement);
        pos = str.find(pattern);
    }

    return str;
}
inline std::string& Lud::inplace::Replace(std::string& str, char pattern, char replacement)
{
    std::ranges::replace(str, pattern, replacement);

    return str;
}

#endif //! LUD_PARSE_HEADER
