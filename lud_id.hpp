#ifndef LUD_UTILS_UUID_HEADER
#define LUD_UTILS_UUID_HEADER
#define TRACE_UUID_LIFETIMES

#include <cstdint>
#include <format>
#include <string>

namespace Lud
{
class UUID
{
public:
	UUID();
	UUID(const UUID& id);
	UUID(UUID&& id) noexcept;
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
struct std::formatter<Lud::uuid_t> : std::formatter<std::string>
{
	template<typename FormatContext>
	auto format(Lud::uuid_t id, FormatContext& ctx) const
	{
		return std::format_to(ctx.out(), "[{:s}]", to_string(id));
	}
};


// implementation
#include <print>
#include <random>
namespace Lud
{
namespace Detail
{

static std::random_device rnd_dev;
static std::mt19937_64 mt(rnd_dev());
static std::uniform_int_distribution<uint64_t> dist;

}

inline UUID::UUID()
	: m_lo(Detail::dist(Detail::mt))
	, m_hi(Detail::dist(Detail::mt))
{
#ifdef TRACE_UUID_LIFETIMES
	std::println("Created UUID {{\033[38;5;46m{:s}\033[31;1;0m}}", to_string(*this));
#endif//TRACE_UUID_LIFETIMES
}

inline UUID::UUID(const UUID& id)
	: m_lo(id.m_lo)
	, m_hi(id.m_hi)
{
#ifdef TRACE_UUID_LIFETIMES
	std::println("Copied UUID {{\033[38;5;46m{:s}\033[31;1;0m}}", to_string(*this));
#endif//TRACE_UUID_LIFETIMES
}

inline UUID::UUID(UUID&& id) noexcept
	: m_lo(std::move(id.m_lo))
	, m_hi(std::move(id.m_hi))
{
#ifdef TRACE_UUID_LIFETIMES
	std::println("Moved UUID {{\033[38;5;126m{:s}\033[31;1;0m}}", to_string(*this));
#endif//TRACE_UUID_LIFETIMES
}

inline UUID::~UUID()
{
#ifdef TRACE_UUID_LIFETIMES
	std::println("Deleted UUID {{\033[38;5;196m{:s}\033[31;1;0m}}", to_string(*this));
#endif//TRACE_UUID_LIFETIMES
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


#endif