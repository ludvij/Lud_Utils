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
#include <type_traits>
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

/**
 * @brief functor to be used to transform collection of collections to collection of size
 *
 * @tparam CollectionT type of the collection
 */
template <sized_collection CollectionT>
struct as_size
{
    size_t operator()(const CollectionT& c) const
    {
        return c.size();
    }
};

// convenient functions

/**
 * @brief reads whole file from stream, will read from current stream position
 *
 * @tparam R range to return file in
 * @param stream stream to read file from
 * @return R range containing file
 */
template <slurpable_range R = std::string>
R SlurpStream(std::istream& stream);

/**
 * @brief read whole file from path
 *
 * @tparam R range to return the file in
 * @tparam T path type, must be string, char* or path
 * @param path path to the file
 * @return R range containing file
 */
template <slurpable_range R = std::string, typename T = const char*>
    requires std::same_as<const std::string&, T> ||
             std::same_as<const std::filesystem::path&, T> ||
             std::same_as<const char*, T>
R Slurp(T path, std::ios_base::openmode = std::ios::binary);

/**
 * @brief Obtains the power set of a given range
 *
 * @tparam RangeT type of the range
 * @tparam T inner type of the range
 * @param r the range
 * @return the power set in vector form
 */
template <std::ranges::input_range RangeT, typename T = std::ranges::range_value_t<RangeT>>
    requires std::same_as<T, std::ranges::range_value_t<RangeT>>
std::vector<std::vector<T>> combinations(const RangeT& r);

/**
 * @brief somewhat good function for hashing a vector of integers
 *
 * @tparam T integer type
 */
template <typename T>
    requires std::is_integral_v<T>
struct HashNumericVector
{
    std::size_t operator()(const std::vector<T>& vec) const;
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

template <std::ranges::input_range RangeT, typename T>
    requires std::same_as<T, std::ranges::range_value_t<RangeT>>
std::vector<std::vector<T>> combinations(const RangeT& r)
{
    std::vector<std::vector<T>> res;

    res.reserve((1 << r.size()) - 1);

    for (const auto& elem : r)
    {
        for (auto set : res)
        {
            set.push_back(elem);
            res.emplace_back(set);
        }
        res.emplace_back(std::vector<T>{elem});
    }
    res.emplace_back();
    return res;
}

template <typename T>
    requires std::is_integral_v<T>
std::size_t HashNumericVector<T>::operator()(const std::vector<T>& vec) const
{
    // https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector/72073933#72073933
    std::size_t seed = vec.size();
    for (auto x : vec)
    {
        x = (x ^ (x >> 16)) * 0x45d9f3bU;
        x = (x ^ (x >> 16)) * 0x45d9f3bU;
        x = (x ^ (x >> 16));
        seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

template <>
std::size_t HashNumericVector<u64>::operator()(const std::vector<u64>& vec) const
{
    // https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector/72073933#72073933
    std::size_t seed = vec.size();
    for (auto x : vec)
    {
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9UL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebUL;
        x = x ^ (x >> 31);
        seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

} // namespace Lud

#endif //! LUD_MISC_HEADER
