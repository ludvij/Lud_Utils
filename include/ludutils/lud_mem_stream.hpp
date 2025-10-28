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

	
#define LUD_READ_BINARY_PTR(stream, ptr, sz) stream.read(std::bit_cast<char*>(ptr), sz)
#define LUD_READ_BINARY(stream, var) LUD_READ_BINARY_PTR(stream, &var, sizeof var)

#define LUD_WRITE_BINARY_PTR(stream, ptr, sz) stream.write(std::bit_cast<char*>(ptr), sz)
#define LUD_WRITE_BINARY(stream, var) LUD_WRITE_BINARY_PTR(stream, &var, sizeof var)

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
	using Base = std::streambuf;

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

	vector_wrap_streambuf(std::vector<T>& buffer, std::ios_base::openmode);
	~vector_wrap_streambuf();
	
protected:

	virtual int_type overflow(int_type ch = traits_type::eof()) override;

	virtual std::streamsize xsputn(const char_type* s, std::streamsize count) override;

	virtual int sync() override;

	virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override;
	virtual pos_type seekpos(pos_type pos, std::ios_base::openmode which) override;

private:
	constexpr void set_pg_area_pointers();

	constexpr void increase_capacity(size_t sz);

	pos_type seekoff_get_area(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which);
	pos_type seekoff_put_area(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which);

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





template<ByteType T>
view_streambuf<T>::view_streambuf(std::span<T> data)
{
	Link(data);
}

template<ByteType T>
view_streambuf<T>::pos_type view_streambuf<T>::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which)
{
	if (which & std::ios_base::out)
	{
		throw std::runtime_error("Writing is currently not supported!");
	}

	if (dir == std::ios_base::cur)
	{
		Base::gbump(static_cast<int>( off ));
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


template<ByteType T>
view_streambuf<T>::pos_type view_streambuf<T>::seekpos(pos_type pos, std::ios_base::openmode which)
{
	return seekoff(pos, std::ios::beg, which);
}


template<ByteType T>
view_streambuf<T>::int_type view_streambuf<T>::underflow()
{
	// reached end of file
	return traits_type::eof();
}



template <ByteType T>
void view_streambuf<T>::Link(std::span<T> data)
{
	char* cs = std::bit_cast<char*>(data.data());
	setg(cs, cs, cs + data.size());
}


template<ByteType T>
memory_istream<T>::memory_istream(std::span<T> data)
	: std::istream(&m_buffer)
	, m_buffer(data)
{
	rdbuf(&m_buffer);
}



template <ByteType T>
void memory_istream<T>::Link(std::span<T> data)
{
	m_buffer.Link(data);
}



template <ByteType T>
vector_wrap_streambuf<T>::vector_wrap_streambuf(std::vector<T> &buffer, std::ios_base::openmode mode)
	: m_buffer(buffer)
{
	if ((mode & std::ios::trunc) && !m_buffer.empty())
	{
		m_buffer.clear();
		m_buffer.shrink_to_fit();
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
	Base::setg(cs, cs, Base::pptr());
}

template <Lud::ByteType T>
Lud::vector_wrap_streambuf<T>::~vector_wrap_streambuf()
{
	sync();
}




template<ByteType T>
constexpr void vector_wrap_streambuf<T>::set_pg_area_pointers()
{
	char* cs = std::bit_cast<char*>(m_buffer.data());

	// can't use m_buffer::size in case of append mode since append mode should not allow repositioning 
	// pointers before original eof
	const ptrdiff_t p_dist = Base::pptr() - Base::pbase();
	const ptrdiff_t g_dist = Base::gptr() - Base::eback();

	if (m_append)
	{
		Base::setp(cs + m_original_size, cs + m_original_size + m_buffer.capacity());
	}
	else
	{
		Base::setp(cs, cs + m_buffer.capacity());
	}

	Base::pbump(p_dist);

	Base::setg(cs, cs + g_dist, Base::pptr());
}

template<ByteType T>
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
		set_pg_area_pointers();
	}
	m_buffer.resize(m_buffer.size() + sz);

}

template<ByteType T>
std::streamsize vector_wrap_streambuf<T>::xsputn(const vector_wrap_streambuf<T>::char_type* s, std::streamsize count)
{
	try 
	{
		increase_capacity(count);
	}
	catch(const std::bad_alloc& e)
	{
		
	}


	std::streamsize written = Base::xsputn(s, count);
	Base::setg(Base::eback(), Base::gptr(), Base::pptr());

	return written;
}

// https://gist.github.com/stephanlachnit/272e5eb82c0e2a1cccd55af5d6b73e2b
template<ByteType T>
vector_wrap_streambuf<T>::int_type vector_wrap_streambuf<T>::overflow(vector_wrap_streambuf<T>::int_type ch)
{
	try 
	{
		increase_capacity(sizeof(char_type));
	}
	catch (const std::bad_alloc& e)
	{
		return traits_type::eof();
	}

	if (!traits_type::eq_int_type(ch, traits_type::eof()))
	{
		// char_type ch_char = traits_type::to_char_type(ch);
		// traits_type::copy(pptr(), &ch_char, 1);
		// https://eel.is/c++draft/streambuf.virtuals#streambuf.virt.put-4.3
		// setp(pptr(), epptr());
		m_buffer[pptr() - pbase()] = static_cast<T>(ch);
		pbump(sizeof(char_type));
	}
	return traits_type::not_eof(ch);
}



template<ByteType T>
int vector_wrap_streambuf<T>::sync()
{
	m_buffer.shrink_to_fit();
	set_pg_area_pointers();

	return 0;
}


template<ByteType T>
vector_wrap_streambuf<T>::pos_type vector_wrap_streambuf<T>::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which)
{
	if (which & std::ios::in)
	{
		return seekoff_get_area(off, dir, which);
	}
	else if (which & std::ios::out)
	{
		if (dir == std::ios_base::cur)
		{
			Base::pbump(static_cast<int>(off));
		}
		else if (dir == std::ios::end)
		{
			char* cs = std::bit_cast<char*>(m_buffer.data());
			Base::setp(cs, cs + m_buffer.capacity());
			Base::pbump(m_buffer.capacity() + off);
		}
		else if (dir == std::ios::beg)
		{
			char* cs = std::bit_cast<char*>(m_buffer.data());
			Base::setp(cs, cs + m_buffer.capacity());
			Base::pbump(static_cast<int>(off));
		}
		return Base::pptr() - Base::pbase();
	}
	return 0;
	
}

template<ByteType T>
vector_wrap_streambuf<T>::pos_type vector_wrap_streambuf<T>::seekpos(pos_type pos, std::ios_base::openmode which)
{
	return seekoff(pos, std::ios::beg, which);
}



template <ByteType T>
vector_wrap_streambuf<T>::pos_type vector_wrap_streambuf<T>::seekoff_get_area(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which)
{
	switch (dir)
	{
	case std::ios::cur: 
	{
		if (gptr() + off > egptr())
		{
			throw std::runtime_error("get ptr offset out of bounds");
		}
		Base::gbump(static_cast<int>( off ));
		break;
	}	
	case std::ios::end: 
	{
		if (off > 0)
		{
			throw std::runtime_error("offset can not be positive while seeking from end");
		}
		Base::setg(Base::eback(), Base::egptr() + off, Base::egptr());
		break;
	} 
	case std::ios::beg: 
	{
		if (off > Base::egptr() - Base::eback())
		{
			throw std::runtime_error("get ptr offset out of bounds");
		}
		Base::setg(Base::eback(), Base::eback() + off, Base::egptr());
		break;
	} 
	}
	return Base::gptr() - Base::eback();
}


}


#endif//!LUD_MEMORY_STREAM_HEADER