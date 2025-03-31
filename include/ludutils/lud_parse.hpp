#ifndef LUD_PARSE_HEADER
#define LUD_PARSE_HEADER

#include <charconv>
#include <optional>
#include <string_view>
#include <algorithm>
#include <cwctype>
#include <ranges>

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

namespace _detail_
{
	// https://stackoverflow.com/a/72451771
	// decay removes const, ref and volatile from type
	template<typename T> inline constexpr bool is_string_class_decayed = false;
	template<typename... T> inline constexpr bool is_string_class_decayed<std::basic_string<T...>> = true;
	template<typename T> inline constexpr bool is_string_class = is_string_class_decayed<std::decay_t<T>>;
	
	template<typename T> inline constexpr bool is_string_view_class_decayed = false;
	template<typename... T> inline constexpr bool is_string_view_class_decayed<std::basic_string_view<T...>> = true;
	template<typename T> inline constexpr bool is_string_view_class = is_string_view_class_decayed<std::decay_t<T>>;
}


// a range that is not a basic_string or a basic_string_view specialization
template<typename T> concept NonStringContainer = requires( T param )
{
	requires std::ranges::range<T>;
	requires !_detail_::is_string_class<T>;
	requires !_detail_::is_string_view_class<T>;
};

template<IntegerType N> N parse_num(std::string_view sv, int base=10);
template<RealType N>    N parse_num(std::string_view sv, std::chars_format fmt=std::chars_format::general);

template<IntegerType N> std::optional<N> is_num(std::string_view sv, int base=10);
template<RealType N>    std::optional<N> is_num(std::string_view sv, std::chars_format fmt=std::chars_format::general);


// TODO: look how to template this
std::vector<std:: string>  Split(const std:: string_view  str, const std:: string_view  delim, size_t n = 0);
std::vector<std::wstring> wSplit(const std::wstring_view wstr, const std::wstring_view wdelim, size_t n = 0);

template<NonStringContainer  Container> std:: string  Join( Container  container, const std:: string_view  delim);
template<NonStringContainer wContainer> std::wstring wJoin(wContainer wcontainer, const std::wstring_view wdelim);

template<typename  Iterator> std:: string  Join( Iterator  first,  Iterator  last, const std:: string_view  delim);
template<typename wIterator> std::wstring wJoin(wIterator wfirst, wIterator wlast, const std::wstring_view wdelim);

std:: string  RemovePrefix(const std:: string_view  str, const std:: string_view  prefix);
std::wstring wRemovePrefix(const std::wstring_view wstr, const std::wstring_view wprefix);

std:: string  RemoveSuffix(const std:: string_view  str, const std:: string_view  suffix);
std::wstring wRemoveSuffix(const std::wstring_view wstr, const std::wstring_view wsuffix);

std:: string  ToUpper(const std:: string_view  str);
std::wstring wToUpper(const std::wstring_view wstr);

std:: string  ToLower(const std:: string_view  str);
std::wstring wToLower(const std::wstring_view wstr);

std:: string  ToTitle(const std:: string_view  str);
std::wstring wToTitle(const std::wstring_view wstr);

std:: string  Capitalize(const std:: string_view  str);
std::wstring wCapitalize(const std::wstring_view wstr);

std:: string  LStrip(const std:: string_view  str);
std::wstring wLStrip(const std::wstring_view wstr);

std:: string  RStrip(const std:: string_view  str);
std::wstring wRStrip(const std::wstring_view wstr);

std:: string  Strip(const std:: string_view  str);
std::wstring wStrip(const std::wstring_view wstr);

std:: string  Reverse(const std:: string_view  str);
std::wstring wReverse(const std::wstring_view wstr);

namespace inplace
{
std:: string&  RemovePrefix(std:: string&  str, const std:: string_view  prefix);
std::wstring& wRemovePrefix(std::wstring& wstr, const std::wstring_view wprefix);

std:: string&  RemoveSuffix(std:: string&  str, const std:: string_view  suffix);
std::wstring& wRemoveSuffix(std::wstring& wstr, const std::wstring_view wsuffix);

std:: string&  ToUpper(std:: string&  str);
std::wstring& wToUpper(std::wstring& wstr);

std:: string&  ToLower(std:: string&  str);
std::wstring& wToLower(std::wstring& wstr);

std:: string&  ToTitle(std:: string&  str);
std::wstring& wToTitle(std::wstring& wstr);

std:: string&  Capitalize(std:: string&  str);
std::wstring& wCapitalize(std::wstring& wstr);

std:: string&  LStrip(std:: string&  str);
std::wstring& wLStrip(std::wstring& wstr);

std:: string&  RStrip(std:: string&  str);
std::wstring& wRStrip(std::wstring& wstr);

std:: string&  Strip(std:: string&  str);
std::wstring& wStrip(std::wstring& wstr);

std:: string&  Reverse(std:: string&  str);
std::wstring& wReverse(std::wstring& wstr);
}

}


// implementation==============================================================================

template<Lud::IntegerType N>
N Lud::parse_num(const std::string_view sv, int base/*=10*/)
{
	N val{};
	std::from_chars(sv.data(), sv.data() + sv.size(), val, base);
	return val;
}

template<Lud::IntegerType N>
std::optional<N> Lud::is_num(const std::string_view sv, int base/*=10*/)
{
	N val{};
	if (std::from_chars(sv.data(), sv.data() + sv.size(), val, base).ec == std::errc::invalid_argument)
	{
		return std::nullopt;
	}
	return val;
}


template<Lud::RealType N>
N Lud::parse_num(const std::string_view sv, const std::chars_format fmt/*=std::chars_format::general)*/)
{
	N val{};
	std::from_chars(sv.data(), sv.data() + sv.size(), val, fmt);
	return val;
}

template<Lud::RealType N>
std::optional<N> Lud::is_num(const std::string_view sv, const std::chars_format fmt/*=std::chars_format::general)*/)
{
	N val{};
	if (std::from_chars(sv.data(), sv.data() + sv.size(), val, fmt).ec == std::errc::invalid_argument)
	{
		return std::nullopt;
	}
	return val;
}

template <Lud::NonStringContainer Container>
std::string Lud::Join(Container container, const std::string_view delim)
{
	return Join(container.begin(), container.end(), delim);
}

template <Lud::NonStringContainer wContainer>
std::wstring Lud::wJoin(wContainer wcontainer, const std::wstring_view wdelim)
{
	return Join(wcontainer.begin(), wcontainer.end(), wdelim);
}

template <typename Iterator>
std::string Lud::Join(Iterator first, Iterator last, const std::string_view delim)
{
	std::string res;
	if (first == last)
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

template <typename wIterator>
std::wstring Lud::wJoin(wIterator wfirst, wIterator wlast, const std::wstring_view wdelim)
{
	std::wstring res;
	if (wfirst == wlast)
	{
		return res;
	}
	res.append(static_cast<std::wstring>(*wfirst));
	for (auto it = wfirst + 1; it != wlast; ++it)
	{
		res.append(wdelim);
		res.append(static_cast<std::wstring>(*it));
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

inline std::vector<std::string> Lud::Split(const std::string_view str, const std::string_view delim, size_t n/*=0*/)
{
	std::vector<std::string> parts;
	size_t next = str.find(delim);
	std::string_view inner_str = str;

	while (next != std::string_view::npos && (n != parts.size() || n == 0))
	{
		if (next != 0)
		{
			parts.emplace_back(inner_str.substr(0, next));
		}
		inner_str.remove_prefix(next + delim.size());
		next = inner_str.find(delim);
	}

	parts.emplace_back(inner_str);

	return parts;
}

inline std::vector<std::wstring> Lud::wSplit(const std::wstring_view wstr, const std::wstring_view wdelim, size_t n/*=0*/)
{
	std::vector<std::wstring> parts;
	size_t next = wstr.find(wdelim);
	std::wstring_view inner_str = wstr;

	while (next != std::wstring_view::npos && (n != parts.size() || n == 0))
	{
		if (next != 0)
		{
			parts.emplace_back(inner_str.substr(0, next));
		}
		inner_str.remove_prefix(next + wdelim.size());
		next = inner_str.find(wdelim);
	}

	parts.emplace_back(inner_str);

	return parts;
}

inline std::wstring Lud::wRemovePrefix(const std::wstring_view wstr, const std::wstring_view wprefix)
{

	if (wstr.starts_with(wprefix))
	{
		return std::wstring(wstr.substr(wprefix.size()));
	}

	return std::wstring(wstr);
}

inline std::wstring Lud::wRemoveSuffix(const std::wstring_view wstr, const std::wstring_view wsuffix)
{
	if (wstr.ends_with(wsuffix))
	{
		return std::wstring(wstr.substr(0, wstr.size() - wsuffix.size()));
	}

	return std::wstring(wstr);
}

inline std::wstring& Lud::inplace::wRemovePrefix(std::wstring& wstr, const std::wstring_view wprefix)
{
	if (wstr.starts_with(wprefix))
	{
		wstr.erase(0, wprefix.size());
	}
	return wstr;
}

inline std::wstring& Lud::inplace::wRemoveSuffix(std::wstring& wstr, const std::wstring_view wsuffix)
{
	if (wstr.ends_with(wsuffix))
	{
		wstr.erase(wstr.size() - wsuffix.size());
	}
	return wstr;
}

inline std::string Lud::ToUpper(const std::string_view str)
{
	std::string res(str);

	inplace::ToUpper(res);

	return res;
}

inline std::wstring Lud::wToUpper(const std::wstring_view wstr)
{
	std::wstring res(wstr);

	inplace::wToUpper(res);

	return res;
}

inline std::string Lud::ToLower(const std::string_view str)
{
	std::string res(str);

	inplace::ToLower(res);

	return res;
}

inline std::wstring Lud::wToLower(const std::wstring_view wstr)
{
	std::wstring res(wstr);

	inplace::wToLower(res);

	return res;
}

inline std::string Lud::ToTitle(const std::string_view str)
{
	std::string res(str);

	inplace::ToTitle(res);

	return res;
}

inline std::wstring Lud::wToTitle(const std::wstring_view wstr)
{
	std::wstring res(wstr);

	inplace::wToTitle(res);

	return res;
}

inline std::string Lud::Capitalize(const std::string_view str)
{
	std::string res(str);

	inplace::Capitalize(res);

	return res;
}

inline std::wstring Lud::wCapitalize(const std::wstring_view wstr)
{
	std::wstring res(wstr);

	inplace::wCapitalize(res);

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

inline std::wstring Lud::wLStrip(const std::wstring_view wstr)
{
	const auto delims = L"\t\n\r ";
	const auto idx = wstr.find_first_not_of(delims);

	if (idx == std::wstring::npos)
	{
		return {};
	}

	return std::wstring(wstr.substr(idx));
}

inline std::string Lud::RStrip(const std::string_view str)
{
	const auto delims = "\t\n\r ";
	const auto idx = str.find_last_not_of(delims);

	if (idx == std::string::npos)
	{
		return {};
	}

	return std::string(str.substr(0, idx));
}

inline std::wstring Lud::wRStrip(const std::wstring_view wstr)
{
	const auto delims = L"\t\n\r ";
	const auto idx = wstr.find_last_not_of(delims);

	if (idx == std::wstring::npos)
	{
		return {};
	}

	return std::wstring(wstr.substr(0, idx));
}

inline std::string Lud::Strip(const std::string_view str)
{
	std::string res(str);

	inplace::Strip(res);

	return res;
}

inline std::wstring Lud::wStrip(const std::wstring_view wstr)
{
	std::wstring res(wstr);

	inplace::wStrip(res);

	return res;
}

inline std::string Lud::Reverse(const std::string_view str)
{
	return std::string(str.rbegin(), str.rend());
}

inline std::wstring Lud::wReverse(const std::wstring_view wstr)
{
	return std::wstring(wstr.rbegin(), wstr.rend());
}

inline std::string& Lud::inplace::ToUpper(std::string& str)
{
	std::ranges::transform(str, str.begin(), [](char c)
		{
			return std::toupper(c);
		});

	return str;
}

inline std::wstring& Lud::inplace::wToUpper(std::wstring& wstr)
{
	std::ranges::transform(wstr, wstr.begin(), [](wchar_t c)
		{
			return std::towupper(c);
		});
	return wstr;
}

inline std::string& Lud::inplace::ToLower(std::string& str)
{
	std::ranges::transform(str, str.begin(), [](char c)
		{
			return std::tolower(c);
		});
	return str;
}

inline std::wstring& Lud::inplace::wToLower(std::wstring& wstr)
{
	std::ranges::transform(wstr, wstr.begin(), [](wchar_t c)
		{
			return std::towlower(c);
		});
	return wstr;
}

inline std::string& Lud::inplace::Capitalize(std::string& str)
{
	if (!str.empty())
	{
		str[0] = static_cast<char>( std::toupper(str[0]) );
	}
	return str;
}

inline std::wstring& Lud::inplace::wCapitalize(std::wstring& wstr)
{
	if (!wstr.empty())
	{
		wstr[0] = static_cast<wchar_t>( std::towupper(wstr[0]) );
	}
	return wstr;
}
inline std::string& Lud::inplace::ToTitle(std::string& str)
{
	const auto delim = "\t\n\r ";
	auto idx = str.find_first_not_of(delim);
	while (idx != std::string::npos)
	{
		str[idx] = static_cast<char>( std::toupper(str[idx]) );
		idx = str.find_first_of(delim, idx + 1);
		if (idx != std::string::npos)
		{
			idx = str.find_first_not_of(delim, idx + 1);
		}
	}
	return str;
}

inline std::wstring& Lud::inplace::wToTitle(std::wstring& wstr)
{
	const auto delim = L"\t\n\r ";
	auto idx = wstr.find_first_not_of(delim);
	while (idx != std::string::npos)
	{
		wstr[idx] = static_cast<wchar_t>( std::towupper(wstr[idx]) );
		idx = wstr.find_first_not_of(delim, idx + 1);
	}
	return wstr;
}

inline std::string& Lud::inplace::LStrip(std::string& str)
{
	const auto delims = "\t\n\r ";
	const auto idx = str.find_first_not_of(delims);

	str.erase(0, idx);
	return str;
}

inline std::wstring& Lud::inplace::wLStrip(std::wstring& wstr)
{
	const auto delims = L"\t\n\r ";
	const auto idx = wstr.find_first_not_of(delims);

	wstr.erase(0, idx);
	return wstr;
}

inline std::string& Lud::inplace::RStrip(std::string& str)
{
	const auto delims = "\t\n\r ";
	const auto idx = str.find_last_not_of(delims);

	if (idx == std::string::npos)
	{
		str.clear();
	}
	else
	{
		str.erase(idx + 1);
	}
	return str;
}

inline std::wstring& Lud::inplace::wRStrip(std::wstring& wstr)
{
	const auto delims = L"\t\n\r ";
	const auto idx = wstr.find_last_not_of(delims);

	if (idx == std::wstring::npos)
	{
		wstr.clear();
	}
	else
	{
		wstr.erase(idx + 1);
	}
	return wstr;
}

inline std::string& Lud::inplace::Strip(std::string& str)
{
	const auto delims = "\t\n\r ";
	const auto first = str.find_first_not_of(delims);
	const auto last = str.find_last_not_of(delims);

	if (first == std::string::npos)
	{
		str.clear();
	}
	else
	{
		str.erase(0, first);
		str.erase(last);
	}
	return str;
}

inline std::wstring& Lud::inplace::wStrip(std::wstring& wstr)
{
	const auto delims = L"\t\n\r ";
	const auto first = wstr.find_first_not_of(delims);
	const auto last = wstr.find_last_not_of(delims);

	if (first == std::wstring::npos)
	{
		wstr.clear();
	}
	else
	{
		wstr.erase(0, first);
		wstr.erase(last);
	}
	return wstr;
}

inline std::string& Lud::inplace::Reverse(std::string& str)
{
	std::reverse(str.begin(), str.end());
	return str;
}

inline std::wstring& Lud::inplace::wReverse(std::wstring& wstr)
{
	std::reverse(wstr.begin(), wstr.end());
	return wstr;
}

#endif//!LUD_PARSE_HEADER