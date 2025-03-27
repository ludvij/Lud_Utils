#ifndef LUD_UTILS_UUID_HEADER
#define LUD_UTILS_UUID_HEADER

#include <cstdint>
#include <format>
#include <string>


// right now this is only to trace lifetimes
namespace Lud
{
class UUID
{
public:
	UUID();
	UUID(const UUID& id);
	UUID(const UUID&& id) noexcept;
	~UUID();


	std::strong_ordering operator<=>(const UUID& other) const = default;

	friend std::string to_string(const UUID& id);

private:
	uint64_t m_lo;
	uint64_t m_hi;
};

using uuid_t = UUID;

}
template <>
struct std::formatter<Lud::uuid_t>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const Lud::uuid_t& id, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{:s}", to_string(id));
	}
};

#ifdef LUD_ID_IMPLEMENTATION

// implementation
#include <print>
#include <random>
namespace Lud
{
namespace _detail_
{

static std::random_device rnd_dev;
static std::mt19937_64 mt(rnd_dev());
static std::uniform_int_distribution<uint64_t> dist;

}

inline UUID::UUID()
	: m_lo(_detail_::dist(_detail_::mt))
	, m_hi(_detail_::dist(_detail_::mt))
{
#ifdef LUD_TRACE_ID_LIFETIMES
	std::println(" Created UUID {{\033[38;5;46m{}\033[31;1;0m}}", *this);
#endif//LUD_TRACE_ID_LIFETIMES
}

inline UUID::UUID(const UUID& id)
	: m_lo(_detail_::dist(_detail_::mt))
	, m_hi(_detail_::dist(_detail_::mt))
{
#ifdef LUD_TRACE_ID_LIFETIMES
	std::println("  Copied UUID {{\033[38;5;46m{}\033[31;1;0m}}", id);
	std::println("     new UUID {{\033[38;5;46m{}\033[31;1;0m}}", *this);
#endif//LUD_TRACE_ID_LIFETIMES
}

inline UUID::UUID(const UUID&& id) noexcept
	: m_lo(_detail_::dist(_detail_::mt))
	, m_hi(_detail_::dist(_detail_::mt))
{
#ifdef LUD_TRACE_ID_LIFETIMES
	std::println("   Moved UUID {{\033[38;5;126m{}\033[31;1;0m}}", id);
	std::println("     new UUID {{\033[38;5;46m{}\033[31;1;0m}}", *this);
#endif//LUD_TRACE_ID_LIFETIMES
}

inline UUID::~UUID()
{
#ifdef LUD_TRACE_ID_LIFETIMES
	std::println(" Deleted UUID {{\033[38;5;196m{}\033[31;1;0m}}", *this);
#endif//LUD_TRACE_ID_LIFETIMES
}

inline std::string to_string(const UUID& id)
{
	return std::format("{:08x}-{:04x}-{:04x}-{:04x}-{:012x}",
		( id.m_hi & 0xFFFF'FFFF'0000'0000 ) >> 32,
		( id.m_hi & 0x0000'0000'FFFF'0000 ) >> 16,
		( id.m_hi & 0x0000'0000'0000'FFFF ) >> 0,
		( id.m_lo & 0xFFFF'0000'0000'0000 ) >> 48,
		( id.m_lo & 0x0000'FFFF'FFFF'FFFF ) >> 0
	);
}

}

#endif//LUD_ID_IMPLEMENTATION
#endif