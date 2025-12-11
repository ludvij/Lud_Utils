#ifndef LUD_MISC_HEADER
#define LUD_MISC_HEADER

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <ranges>
#include <vector>

// more convenient ints

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

namespace Lud {

// some usefull concepts
template <typename RangeT, typename InnerT>
concept range_of = requires(RangeT r, InnerT t) {
    requires std::ranges::range<RangeT>;
    requires std::same_as<std::ranges::range_value_t<RangeT>, InnerT>;
};

template <typename RangeT, size_t sz>
concept range_contains_of_size = requires(RangeT r) {
    requires std::ranges::range<RangeT>;
    requires sizeof(std::ranges::range_value_t<RangeT>) == sz;
};

template <typename RangeT, typename InnerT>
concept insertable_range = requires(RangeT r, InnerT t) {
    requires range_of<RangeT, InnerT>;
    requires std::ranges::input_range<RangeT>;
    { std::inserter(r, r.end()) };
};

template <typename RangeT>
concept slurpable_range = requires(RangeT) {
    requires range_contains_of_size<RangeT, 1>;
    requires insertable_range<RangeT, std::ranges::range_value_t<RangeT>>;
};

template <typename CollectionT>
concept sized_collection = requires(CollectionT c) {
    { c.size() } -> std::same_as<size_t>;
};

template <sized_collection CollectionT>
struct as_size
{
    size_t operator()(const CollectionT& c) const
    {
        return c.size();
    }
};

// convenient functions

template <slurpable_range R = std::string>
R SlurpStream(std::istream& stream);

template <slurpable_range R = std::string, typename T = const char*>
    requires std::same_as<const std::string&, T> ||
             std::same_as<const std::filesystem::path&, T> ||
             std::same_as<const char*, T>
R Slurp(T path, std::ios_base::openmode = std::ios::binary);

/**
 * @brief to be used as a functor inside a range transform or something like that
 *
 * @tparam T object to be constructed
 */
template <typename T>
struct constructor
{
    template <typename... Args>
    T operator()(Args&&... args)
    {
        return T(std::forward<Args>(args)...);
    }
};

} // namespace Lud

// IMPLEMENTATION
namespace Lud {

template <slurpable_range R>
R SlurpStream(std::istream& stream)
{
    using InnerT = std::ranges::range_value_t<R>;

    const auto original_pos = stream.tellg();
    stream.seekg(0, std::ios::end);
    const auto size = stream.tellg();
    stream.seekg(original_pos);

    std::vector<InnerT> vec(size);

    stream.read(reinterpret_cast<char*>(vec.data()), size);

    return R{vec.begin(), vec.end()};
}

template <>
std::vector<uint8_t> SlurpStream(std::istream& stream)
{

    const auto original_pos = stream.tellg();
    stream.seekg(0, std::ios::end);
    const auto size = stream.tellg();
    stream.seekg(original_pos);

    std::vector<uint8_t> vec(size);

    stream.read(reinterpret_cast<char*>(vec.data()), size);

    return vec;
}

template <slurpable_range R, typename T>
    requires std::same_as<const std::string&, T> ||
             std::same_as<const std::filesystem::path&, T> ||
             std::same_as<const char*, T>
R Slurp(T path, std::ios_base::openmode mode)
{
    std::ifstream file(path, mode);
    file.exceptions(std::ios::badbit);

    if (!file)
    {
        throw std::runtime_error("[ERROR]: could not open file");
    }

    return SlurpStream<R>(file);
}

} // namespace Lud

#endif //! LUD_MISC_HEADER
