#ifndef LUD_MEMORY_STREAM_HEADER
#define LUD_MEMORY_STREAM_HEADER
#include <ios>
#include <istream>

#include <cstdint>
#include <iostream>
#include <span>
#include <streambuf>
#include <vector>

namespace Lud {

template <typename T>
concept ByteType = requires(T param) {
    requires sizeof param == 1;
};

template <typename R, typename C = uint8_t>
concept BinaryRange = requires {
    requires ByteType<C>;
    requires std::ranges::forward_range<R>;
    requires std::same_as<std::ranges::range_value_t<R>, C>;
};

// C++ standard does not provide char_traits specification for
// uint8_t and recommends just casting when using binary data
template <ByteType T = uint8_t>
class view_streambuf : public std::streambuf
{
public:
    using Base = std::streambuf;
    using IntT = Base::int_type;
    using TraitsT = Base::traits_type;
    using CharT = Base::char_type;
    using OffT = Base::off_type;
    using PosT = Base::pos_type;

    view_streambuf(std::span<const T> data);
    view_streambuf() = default;

protected:
    PosT seekoff(OffT off, std::ios_base::seekdir dir, std::ios_base::openmode which) override;
    PosT seekpos(PosT pos, std::ios_base::openmode which) override;

    std::streamsize xsgetn(CharT* s, std::streamsize count) override;
};

template <ByteType T = uint8_t>
class memory_istream : public std::istream
{
public:
    memory_istream(std::span<const T> data);

    memory_istream() = default;

private:

    view_streambuf<T> m_buffer;
};

template <ByteType T = uint8_t>
class vector_wrap_streambuf : public std::streambuf
{
public:
    using Base = std::streambuf;
    using IntT = Base::int_type;
    using TraitsT = Base::traits_type;
    using CharT = Base::char_type;
    using OffT = Base::off_type;
    using PosT = Base::pos_type;

    vector_wrap_streambuf(std::vector<T>& buffer, std::ios_base::openmode);
    ~vector_wrap_streambuf();

protected:

    IntT overflow(IntT ch = TraitsT::eof()) override;

    std::streamsize xsputn(const CharT* s, std::streamsize count) override;

    int sync() override;

    PosT seekoff(OffT off, std::ios_base::seekdir dir, std::ios_base::openmode which) override;
    PosT seekpos(PosT pos, std::ios_base::openmode which) override;

private:
    constexpr void set_pg_area_pointers();

    constexpr void increase_capacity(size_t sz);

    constexpr PosT seekoff_get_area(OffT off, std::ios_base::seekdir dir, std::ios_base::openmode which);
    constexpr PosT seekoff_put_area(OffT off, std::ios_base::seekdir dir, std::ios_base::openmode which);

    constexpr size_t get_put_area_size() const;
    constexpr size_t get_get_area_size() const;

private:
    size_t m_original_size = 0;
    std::vector<T>& m_buffer;
    std::ios_base::openmode m_openmode;
};

template <ByteType T = uint8_t>
class vector_istream : public std::istream
{
public:
    vector_istream(std::vector<T>& vec, std::ios_base::openmode = std::ios::in);

private:
    vector_wrap_streambuf<T> m_sbuf;
};

template <ByteType T = uint8_t>
class vector_ostream : public std::ostream
{
public:
    vector_ostream(std::vector<T>& vec, std::ios_base::openmode = std::ios::out);

private:
    vector_wrap_streambuf<T> m_sbuf;
};

template <ByteType T>
view_streambuf<T>::view_streambuf(std::span<const T> data)
{
    auto cs = std::bit_cast<char*>(data.data());
    Base::setg(cs, cs, cs + data.size());
}

template <ByteType T>
view_streambuf<T>::PosT view_streambuf<T>::seekoff(OffT off, std::ios_base::seekdir dir, std::ios_base::openmode which)
{
    if (dir == std::ios_base::cur)
    {
        Base::gbump(static_cast<int>(off));
    }
    else if (dir == std::ios_base::end)
    {
        Base::setg(Base::eback(), Base::egptr() + off, Base::egptr());
    }
    else if (dir == std::ios_base::beg)
    {
        Base::setg(Base::eback(), Base::eback() + off, Base::egptr());
    }
    return Base::gptr() - Base::eback();
}

template <ByteType T>
view_streambuf<T>::PosT view_streambuf<T>::seekpos(PosT pos, std::ios_base::openmode which)
{
    return seekoff(pos, std::ios::beg, which);
}

template <ByteType T>
std::streamsize view_streambuf<T>::xsgetn(CharT* s, std::streamsize count)
{
    std::streamsize avail = Base::egptr() - Base::gptr();
    auto to_copy = avail < count ? avail : count;
    if (avail == 0)
    {
        return 0;
    }
    TraitsT::copy(s, Base::gptr(), to_copy);
    Base::gbump(to_copy);

    return to_copy;
}

template <ByteType T>
memory_istream<T>::memory_istream(std::span<const T> data)
    : std::istream(&m_buffer)
    , m_buffer(data)
{
}

template <ByteType T>
vector_wrap_streambuf<T>::vector_wrap_streambuf(std::vector<T>& buffer, std::ios_base::openmode mode)
    : m_buffer(buffer)
    , m_openmode(mode)
{
    char* cs = reinterpret_cast<char*>(m_buffer.data());

    if (mode & std::ios::out)
    {
        if (mode & std::ios::trunc)
        {
            m_buffer.clear();
        }
        else if (mode & std::ios::app)
        {
            m_original_size = m_buffer.size();
            Base::setp(cs + m_original_size, cs + m_original_size);
        }
        else
        {
            Base::setp(cs, cs + m_buffer.size());
        }
        if (mode & std::ios::ate)
        {
            Base::pbump(m_buffer.size());
        }
    }
    if (mode & std::ios::in)
    {
        Base::setg(cs, cs, cs + m_buffer.size());

        if (mode & std::ios::ate)
        {
            Base::gbump(m_buffer.size());
        }
    }
}

template <Lud::ByteType T>
vector_wrap_streambuf<T>::~vector_wrap_streambuf()
{
    sync();
}

template <ByteType T>
constexpr void vector_wrap_streambuf<T>::set_pg_area_pointers()
{
    char* cs = reinterpret_cast<char*>(m_buffer.data());

    // can't use m_buffer::size in case of append mode since append mode should not allow repositioning
    // pointers before original eof
    if (m_openmode & std::ios::out)
    {
        const ptrdiff_t p_dist = Base::pptr() - Base::pbase();
        if (m_openmode & std::ios::app)
        {
            const size_t avail_size = m_buffer.size() - m_original_size;
            Base::setp(cs + m_original_size, cs + avail_size);
        }
        else
        {
            Base::setp(cs, cs + m_buffer.size());
        }

        Base::pbump(p_dist);
    }
    if (m_openmode & std::ios::in)
    {
        const ptrdiff_t g_dist = Base::gptr() - Base::eback();

        Base::setg(cs, cs + g_dist, cs + m_buffer.size());
    }
}

template <ByteType T>
constexpr void vector_wrap_streambuf<T>::increase_capacity(size_t sz)
{
    // this is called in xsputn and overflow
    // so in order to reduce reallocations we will be copying
    // std::vector approach and doubling size when we are out of room
    // if there is enough room we will do nothing
    if (m_buffer.size() + sz >= m_buffer.capacity())
    {
        std::size_t next_size = std::max(
            m_buffer.size() + sz,
            static_cast<size_t>(m_buffer.size() * 1.5)
        );
        m_buffer.reserve(next_size);
    }
    m_buffer.resize(m_buffer.size() + sz);
    set_pg_area_pointers();
}

template <ByteType T>
std::streamsize vector_wrap_streambuf<T>::xsputn(const CharT* s, std::streamsize count)
{
    try
    {
        increase_capacity(count);
    }
    catch (const std::bad_alloc& e)
    {
        // feels icky, but if increase_capacity fails Base::xsptun() should end up calling overflow
        // this probably has a shit ton of errors, should be tested
        ;
    }

    std::streamsize written = Base::xsputn(s, count);
    Base::setg(Base::eback(), Base::gptr(), Base::pptr());

    return written;
}

// https://gist.github.com/stephanlachnit/272e5eb82c0e2a1cccd55af5d6b73e2b
template <ByteType T>
vector_wrap_streambuf<T>::IntT vector_wrap_streambuf<T>::overflow(IntT ch)
{
    try
    {
        increase_capacity(sizeof(CharT));
    }
    catch (const std::bad_alloc& e)
    {
        return TraitsT::eof();
    }

    if (!TraitsT::eq_int_type(ch, TraitsT::eof()))
    {
        // CharT ch_char = TraitsT::to_CharT(ch);
        // TraitsT::copy(pptr(), &ch_char, 1);
        // https://eel.is/c++draft/streambuf.virtuals#streambuf.virt.put-4.3
        // setp(pptr(), epptr());
        size_t pos = pptr() - pbase();
        if (m_openmode & std::ios::app)
        {
            pos += m_original_size;
        }
        m_buffer[pos] = static_cast<T>(ch);
        pbump(sizeof(CharT));
    }
    return TraitsT::not_eof(ch);
}

template <ByteType T>
int vector_wrap_streambuf<T>::sync()
{
    m_buffer.shrink_to_fit();
    set_pg_area_pointers();

    return 0;
}

template <ByteType T>
vector_wrap_streambuf<T>::PosT vector_wrap_streambuf<T>::seekoff(OffT off, std::ios_base::seekdir dir, std::ios_base::openmode which)
{
    if (which & std::ios::in)
    {
        return seekoff_get_area(off, dir, which);
    }
    if (which & std::ios::out)
    {
        return seekoff_put_area(off, dir, which);
    }
    throw std::invalid_argument("which must be either std::ios::in or std::ios::out");
}

template <ByteType T>
vector_wrap_streambuf<T>::PosT vector_wrap_streambuf<T>::seekpos(PosT pos, std::ios_base::openmode which)
{
    return seekoff(pos, std::ios::beg, which);
}

template <ByteType T>
constexpr vector_wrap_streambuf<T>::PosT vector_wrap_streambuf<T>::seekoff_get_area(OffT off, std::ios_base::seekdir dir, std::ios_base::openmode which)
{
    switch (dir)
    {
    case std::ios::cur:
    {
        if (gptr() + off > egptr())
        {
            throw std::range_error("get ptr offset out of bounds");
        }
        Base::gbump(static_cast<int>(off));
        break;
    }
    case std::ios::end:
    {
        if (off > 0 || static_cast<size_t>(-off) > get_get_area_size())
        {
            throw std::range_error("get ptr offset out of bounds");
        }
        Base::setg(Base::eback(), Base::egptr() + off, Base::egptr());
        break;
    }
    case std::ios::beg:
    {
        if (off < 0 || static_cast<size_t>(off) > get_get_area_size())
        {
            throw std::range_error("get ptr offset out of bounds");
        }
        Base::setg(Base::eback(), Base::eback() + off, Base::egptr());
        break;
    }
    default:
        break;
    }
    return Base::gptr() - Base::eback();
}

template <ByteType T>
constexpr vector_wrap_streambuf<T>::PosT vector_wrap_streambuf<T>::seekoff_put_area(OffT off, std::ios_base::seekdir dir, std::ios_base::openmode which)
{
    if (dir == std::ios_base::cur)
    {
        if (off + Base::pptr() > Base::epptr())
        {
            throw std::out_of_range("put ptr offset out of range");
        }
        Base::pbump(static_cast<int>(off));
    }
    else if (dir == std::ios::end)
    {
        if (off > 0 || static_cast<size_t>(-off) > get_put_area_size())
        {
            throw std::range_error("put ptr offset out of range");
        }
        Base::setp(Base::pbase(), Base::epptr());
        Base::pbump(m_buffer.capacity() + off);
    }
    else if (dir == std::ios::beg)
    {
        if (off < 0 || static_cast<size_t>(off) > get_put_area_size())
        {
            throw std::range_error("put ptr offset out of range");
        }
        Base::setp(Base::pbase(), Base::epptr());
        Base::pbump(static_cast<int>(off));
    }
    return Base::pptr() - Base::pbase();
}

template <ByteType T>
constexpr size_t vector_wrap_streambuf<T>::get_put_area_size() const
{
    return Base::epptr() - Base::pbase();
}

template <ByteType T>
constexpr size_t vector_wrap_streambuf<T>::get_get_area_size() const
{
    return Base::egptr() - Base::eback();
}

template <ByteType T>
vector_istream<T>::vector_istream(std::vector<T>& vec, std::ios_base::openmode mode)
    : std::istream(&m_sbuf)
    , m_sbuf(vec, mode | std::ios::in)
{
    if (mode & std::ios_base::out)
    {
        throw std::runtime_error("invalid openmode");
    }
}

template <ByteType T>
vector_ostream<T>::vector_ostream(std::vector<T>& vec, std::ios_base::openmode mode)
    : std::ostream(&m_sbuf)
    , m_sbuf(vec, mode | std::ios::out)
{
    if (mode & std::ios_base::in)
    {
        throw std::runtime_error("invalid openmode");
    }
}

} // namespace Lud

#endif //! LUD_MEMORY_STREAM_HEADER
