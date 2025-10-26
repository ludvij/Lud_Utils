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

	virtual int_type underflow() override;
};

template<ByteType T = char>
class vector_wrap_streambuf : public std::streambuf
{
public:
	using Base = std::streambuf;

	vector_wrap_streambuf(std::vector<T>& buffer, std::ios_base::openmode = std::ios::out | std::ios::trunc);
	
protected:

	virtual int_type overflow(int_type ch = traits_type::eof()) override;

	virtual std::streamsize xsputn(const char_type* s, std::streamsize count) override;


private:
	constexpr void set_put_area_pointers();

	constexpr void reserve_additional(size_t sz);

private:
	bool m_append = false;
	size_t m_original_size = 0;
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


template<Lud::ByteType T>
Lud::view_streambuf<T>::int_type Lud::view_streambuf<T>::underflow()
{
	// reached end of file
	return traits_type::eof();
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
Lud::vector_wrap_streambuf<T>::vector_wrap_streambuf(std::vector<T> &buffer, std::ios_base::openmode mode)
	: m_buffer(buffer)
{
	if (mode & std::ios::trunc)
	{
		m_buffer.clear();
	}
	
	char* cs = std::bit_cast<char*>(m_buffer.data());
	
	if (mode & std::ios::app) 
	{
		m_append = true;
		m_original_size = m_buffer.size();
		Base::setp(cs + m_original_size, cs + m_original_size + m_buffer.capacity());
	}
	if (mode & std::ios::ate)
	{
		Base::setp(cs, cs + m_buffer.capacity());
		Base::pbump(m_buffer.size());
	}
}



template<Lud::ByteType T>
Lud::vector_wrap_streambuf<T>::int_type Lud::vector_wrap_streambuf<T>::overflow(Lud::vector_wrap_streambuf<T>::int_type ch)
{
	reserve_additional(1);
	m_buffer.push_back(static_cast<T>(ch));
	pbump(1);
	return ch;
}



template<Lud::ByteType T>
constexpr void Lud::vector_wrap_streambuf<T>::set_put_area_pointers()
{
	char* cs = std::bit_cast<char*>(m_buffer.data());

	// can't use m_buffer::size in case of append mode since append mode should not allow repositioning 
	// pointers before original eof
	const ptrdiff_t dist = Base::pptr() - Base::pbase();

	if (m_append)
	{
		Base::setp(cs + m_original_size, cs + m_original_size + m_buffer.capacity());
	}
	else
	{
		Base::setp(cs, cs + m_buffer.capacity());
	}

	Base::pbump(dist);
}

template<Lud::ByteType T>
constexpr void Lud::vector_wrap_streambuf<T>::reserve_additional(size_t sz)
{
	m_buffer.reserve(m_buffer.size() + sz);
	set_put_area_pointers();
}


template<Lud::ByteType T>
std::streamsize Lud::vector_wrap_streambuf<T>::xsputn(const Lud::vector_wrap_streambuf<T>::char_type* s, std::streamsize count)
{
	reserve_additional(count);
	m_buffer.resize(m_buffer.size() + count);

	traits_type::copy(Base::pptr(), s, count);

	Base::pbump(count);

	return count;
}


#endif//!LUD_MEMORY_STREAM_HEADER