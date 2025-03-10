#ifndef LUD_SLURPER_HEADER
#define LUD_SLURPER_HEADER

#include <ostream>
#include <string>
#include <string_view>

#include <algorithm>
#include <numeric>

#include <filesystem>
#include <fstream>

#include <bit>
#include <deque>
#include <span>
#include <vector>

#include <format>

namespace Lud
{

class Slurper : public std::ifstream
{
public:
	Slurper() = default;
	Slurper(const std::filesystem::path, std::ios_base::openmode mode = std::ios_base::in);

	Slurper(const Slurper& slurper) = delete;
	Slurper(Slurper&& slurper) noexcept = default;


	~Slurper();

	// return remaining file as vector
	std::vector<std::string> ReadLines();

	// reads a single line
	std::string ReadLine();

	// reads size of structure from file to provided structure
	Slurper& Read(char* data, size_t size);
	template<typename T> T Read();


	// returns current pos in file
	std::streampos Where();
	// moves current file pos to provided pos
	Slurper& Move(std::streamoff pos);
	// moves current file pos by an offset in a direction
	Slurper& Move(std::streamoff offset, std::ios_base::seekdir dir);

	size_t Size();

	// returns file pointer to pos 0
	Slurper& Reset();

	// opens file 
	bool Open(const std::filesystem::path, std::ios_base::openmode mode = std::ios_base::in);

	// returns file as vector of lines without you needing to creating a slurper
	static std::vector<std::string> Slurp(const std::filesystem::path, std::ios_base::openmode mode = std::ios_base::in);

	// reads whole file to T vector
	template<typename T> std::vector<T> ReadTo();
	template<typename T> void ReadTo(std::span<T> data);

	template<typename T> static std::vector<T> SlurpTo(const std::filesystem::path, std::ios_base::openmode mode = std::ios_base::in);


	// closes file
	void Close();

	bool HasSpace(size_t size);

private:
	std::ios_base::openmode m_mode{};


};

}

#ifdef LUD_SLURPER_IMPLEMENTATION
namespace Lud
{
inline Slurper::Slurper(const std::filesystem::path filename, const std::ios_base::openmode mode)
	: m_mode(mode)
{
	Open(filename, mode);
}


inline Slurper::~Slurper()
{
	Close();
}


inline std::vector<std::string> Slurper::Slurp(const std::filesystem::path filename, const std::ios_base::openmode mode)
{
	Slurper file(filename, mode);
	return file.ReadLines();
}

inline bool Lud::Slurper::Open(const std::filesystem::path filename, const std::ios_base::openmode mode)
{
	if (is_open())
	{
		Close();
	}

	open(filename, mode);
	return is_open();

}


inline Slurper& Slurper::Reset()
{
	clear();
	seekg(0, std::ios::beg);

	return *this;
}

inline void Slurper::Close()
{
	close();
}

template <typename T>
inline T Slurper::Read()
{
	T dat;
	*this >> dat;
	return dat;
}

inline std::string Slurper::ReadLine()
{
	std::string line;
	std::getline(*this, line);

	return line;
}

inline std::vector<std::string> Slurper::ReadLines()
{
	std::vector<std::string> lines;

	for (std::string line; std::getline(*this, line);)
	{
		lines.emplace_back(line);
	}
	return lines;
}

inline Slurper& Slurper::Read(char* data, size_t size)
{
	read(data, size);
	return *this;
}

inline std::streampos Slurper::Where()
{
	return tellg();
}

inline Slurper& Slurper::Move(const std::streamoff pos)
{
	seekg(pos);

	return *this;
}

inline Slurper& Slurper::Move(const std::streamoff offset, const std::ios_base::seekdir dir)
{
	seekg(offset, dir);

	return *this;
}

inline size_t Slurper::Size()
{
	if (!( m_mode & std::ios::ate ))
		Move(0, std::ios::end);
	const size_t size = static_cast<size_t>( Where() );
	Move(0, std::ios::beg);

	return size;
}

inline bool Slurper::HasSpace(const size_t size)
{
	const size_t current = Where();
	Move(0, std::ios::end);
	const size_t length = Where();
	Move(current);

	return size < length - current;
}

template<typename T>
inline std::vector<T> Slurper::ReadTo()
{
	const size_t file_size = Size();
	std::vector<T> buffer(file_size / sizeof(T));
	read(reinterpret_cast<char*>( buffer.data() ), file_size);

	return buffer;
}
template<typename T>
inline void Slurper::ReadTo(std::span<T> data)
{
	read(std::bit_cast<char*>( data.data() ), data.size_bytes());
}
template<typename T>
inline std::vector<T> Slurper::SlurpTo(const std::filesystem::path, std::ios_base::openmode mode)
{
	Slurper file(filename, mode | std::ios::ate);
	return file.ReadTo<T>();
}

}

#endif
#endif