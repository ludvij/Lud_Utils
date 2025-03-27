#ifndef LUD_MEMORY_STREAM_HEADER
#define LUD_MEMORY_STREAM_HEADER
#define LUD_MEMORY_STREAMING


#include <iostream>
#include <streambuf>

namespace Lud
{

template<typename T>
concept ByteType = requires( T param )
{
	requires sizeof param == 1;
};

template<ByteType T = char>
class memory_streambuf : public std::streambuf
{
public:
	memory_streambuf(T* ptr, size_t size);
protected:
	virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in) override;
	virtual pos_type seekpos(pos_type pos, std::ios_base::openmode which = std::ios_base::in) override;
};

template<ByteType T = char>
class memory_istream : public std::istream
{
public:
	memory_istream(T* s, size_t n);
private:
	memory_streambuf<T> m_buffer;
};
}

#define LUD_READ_BINARY_PTR(stream, ptr, sz) stream.read(std::bit_cast<char*>(ptr), sz)
#define LUD_READ_BINARY(stream, var) LUD_READ_BINARY_PTR(stream, &var, sizeof var)

#define LUD_WRITE_BINARY_PTR(stream, ptr, sz) stream.write(std::bit_cast<char*>(ptr), sz)
#define LUD_WRITE_BINARY(stream, var) LUD_WRITE_BINARY_PTR(stream, &var, sizeof var)

template<Lud::ByteType T>
inline Lud::memory_streambuf<T>::memory_streambuf(T* s, size_t n)
{
	auto cs = std::bit_cast<char*>( s );
	setg(cs, cs, cs + n);
}
template<Lud::ByteType T>
inline Lud::memory_streambuf<T>::pos_type Lud::memory_streambuf<T>::seekoff(off_type off, std::ios_base::seekdir dir, [[maybe_unused]] std::ios_base::openmode which)
{
	if (dir == std::ios_base::cur)
		gbump(static_cast<int>( off ));
	else if (dir == std::ios_base::end)
		setg(eback(), egptr() + off, egptr());
	else if (dir == std::ios_base::beg)
		setg(eback(), eback() + off, egptr());
	return gptr() - eback();
}


template<Lud::ByteType T>
inline Lud::memory_streambuf<T>::pos_type Lud::memory_streambuf<T>::seekpos(pos_type pos, [[maybe_unused]] std::ios_base::openmode which)
{
	setg(eback(), eback() + pos, egptr());
	return gptr() - eback();
}

template<Lud::ByteType T>
inline Lud::memory_istream<T>::memory_istream(T* s, size_t n)
	: std::istream(&m_buffer)
	, m_buffer(s, n)
{
	rdbuf(&m_buffer);
}


#endif//!LUD_MEMORY_STREAM_HEADER