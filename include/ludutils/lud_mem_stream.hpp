#ifndef LUD_MEMORY_STREAM_HEADER
#define LUD_MEMORY_STREAM_HEADER
#define LUD_MEMORY_STREAMING


#include <iostream>
#include <streambuf>
#include <cstdint>
#include <vector>
#include <span>

namespace Lud
{

template<typename T>
concept ByteType = requires( T param )
{
	requires sizeof param == 1;
};



template<typename R, typename C = uint8_t>
concept BinaryRange = requires
{
	requires ByteType<C>;
	requires std::ranges::forward_range<R>;
	requires std::same_as<std::ranges::range_value_t<R>, C>;
};

// C++ standard does not provide char_trais spect for
// uint8_t and recommends just casting when using binary data
template<ByteType T = char>
class view_streambuf : public std::streambuf
{
public:
	view_streambuf(std::span<T> data);
	view_streambuf() = default;

	void Link(std::span<T> data);

protected:
	virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in) override;
	virtual pos_type seekpos(pos_type pos, std::ios_base::openmode which = std::ios_base::in) override;
};

template<ByteType T = char>
class vector_wrap_streambuf : public std::streambuf
{
public:
	vector_wrap_streambuf(std::vector<T>& buffer);

	
protected:
private:
	std::vector<T>& m_buffer;
};

template<ByteType T = char>
class memory_istream : public std::istream
{
public:
	memory_istream(std::span<T> data);

	memory_istream() = default;

	void Link(std::span<T> data);
	
private:
	view_streambuf<T> m_buffer;
};



}

#define LUD_READ_BINARY_PTR(stream, ptr, sz) stream.read(std::bit_cast<char*>(ptr), sz)
#define LUD_READ_BINARY(stream, var) LUD_READ_BINARY_PTR(stream, &var, sizeof var)

#define LUD_WRITE_BINARY_PTR(stream, ptr, sz) stream.write(std::bit_cast<char*>(ptr), sz)
#define LUD_WRITE_BINARY(stream, var) LUD_WRITE_BINARY_PTR(stream, &var, sizeof var)

template<Lud::ByteType T>
Lud::view_streambuf<T>::view_streambuf(std::span<T> data)
{
	Link(data);
}

template<Lud::ByteType T>
Lud::view_streambuf<T>::pos_type Lud::view_streambuf<T>::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which)
{
	if (which & std::ios_base::out)
	{
		throw std::runtime_error("Writing is currently not supported!");
	}

	if (dir == std::ios_base::cur)
	{
		gbump(static_cast<int>( off ));
	}
	else if (dir == std::ios_base::end)
	{
		setg(eback(), egptr() + off, egptr());
	}
	else if (dir == std::ios_base::beg)
	{
		setg(eback(), eback() + off, egptr());
	}
	
	return gptr() - eback();
}


template<Lud::ByteType T>
Lud::view_streambuf<T>::pos_type Lud::view_streambuf<T>::seekpos(pos_type pos, std::ios_base::openmode which)
{
	if (which & std::ios_base::out)
	{
		throw std::runtime_error("Writing is currently not supported!");
	}

	setg(eback(), eback() + pos, egptr());
	return gptr() - eback();
}



template <Lud::ByteType T>
void Lud::view_streambuf<T>::Link(std::span<T> data)
{
	char* cs = std::bit_cast<char*>(data.data());
	setg(cs, cs, cs + data.size());
}


template<Lud::ByteType T>
Lud::memory_istream<T>::memory_istream(std::span<T> data)
	: std::istream(&m_buffer)
	, m_buffer(data)
{
	rdbuf(&m_buffer);
}



template <Lud::ByteType T>
void Lud::memory_istream<T>::Link(std::span<T> data)
{
	m_buffer.Link(data);
}



template <Lud::ByteType T>
Lud::vector_wrap_streambuf<T>::vector_wrap_streambuf(std::vector<T> &buffer)
	: m_buffer(buffer)
{
	const char* cs = std::bit_cast<char*>(m_buffer.data());
	setg(cs, cs, cs + m_buffer.size());
	setp(cs, cs + m_buffer.size());
}

#endif//!LUD_MEMORY_STREAM_HEADER