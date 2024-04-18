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
#include <span>
#include <vector>

#include <format>

namespace Lud
{

class Slurper
{
public:
	Slurper() = default;
	Slurper(const std::string_view filename, std::ios_base::openmode mode = std::ios_base::in);
	Slurper(const char* filename, std::ios_base::openmode mode = std::ios_base::in);

	Slurper(const Slurper& slurper) = delete;
	Slurper(Slurper&& slurper) noexcept = default;


	~Slurper();

	// return remaining file as vector
	std::vector<std::string> ReadLines();

	// reads a single line
	std::string ReadLine();

	// reads size of structure from file to provided structure
	template<typename T> Slurper& ReadChunk(T& t);
	Slurper& Read(char* data, size_t size);
	template<typename T> T Read();


	// returns current pos in file
	std::streampos Where();
	// moves current file pos to provided pos
	Slurper& Move(std::streamoff pos);
	// moves current file pos by an offset in a direction
	Slurper& Move(std::streamoff offset, std::ios_base::seekdir dir);

	// returns file pointer to pos 0
	Slurper& Reset();

	// returns true if file was opened
	bool IsOpen() const;

	// opens file 
	bool Open(const std::string_view filename, std::ios_base::openmode mode = std::ios_base::in);
	bool Open(const char* filename, std::ios_base::openmode mode = std::ios_base::in);

	// returns file as vector of lines without you needing to creating a slurper
	static std::vector<std::string> Slurp(const std::string_view filename, std::ios_base::openmode mode = std::ios_base::in);
	static std::vector<std::string> Slurp(const char* filename, std::ios_base::openmode mode = std::ios_base::in);

	// reads whole file to T vector
	template<typename T> std::vector<T> ReadTo();

	template<typename T> static std::vector<T> SlurpTo(const std::string_view filename, std::ios_base::openmode mode = std::ios_base::in);
	template<typename T> static std::vector<T> SlurpTo(const char* filename, std::ios_base::openmode mode = std::ios_base::in);

	// write funcionality
	template<typename T> Slurper& WriteList(const std::span<T> list, const std::string_view delim, std::optional<size_t> chunk_size = std::nullopt);
	template<typename T> Slurper& WriteList(const std::span<T> list, const char* delim, std::optional<size_t> chunk_size = std::nullopt);

	Slurper& Write(const char* n, std::streampos s);
	template<typename T> Slurper& Write(T dat);
	template<class... Args> Slurper& WriteFmt(const std::format_string<Args...> fmt, Args... args);
	template<typename T> Slurper& WriteListFmt(const std::span<T> list, const std::format_string<T> fmt, const std::string_view delim, std::optional<size_t> chunk_size = std::nullopt);
	template<typename T> Slurper& WriteListFmt(const std::span<T> list, const std::format_string<T> fmt, const char* delim, std::optional<size_t> chunk_size = std::nullopt);

	Slurper& WriteLine(const std::string_view line = "");
	Slurper& WriteLine(const char* line = "");

	template<class... Args> Slurper& WriteFmtLine(const std::format_string<Args...> fmt, Args... args);

	Slurper& WriteLines(const std::span<std::string_view> lines);
	Slurper& WriteLines(const std::span<const char*> lines);

	template<typename T> std::ostream& operator <<(const T& t);
	template<typename T> std::istream& operator >>(T& t);

	Slurper& SetTabLevel(int level);

	Slurper& NewLine();

	// closes file
	void Close();

	bool HasSpace(size_t size);

private:
	std::fstream m_file;
	std::ios_base::openmode m_mode{};

	int m_tab_level = 0;
	std::string m_tabs;

};
}

#ifdef LUD_SLURPER_IMPLEMENTATION
namespace Lud
{
inline Slurper::Slurper(const std::string_view filename, const std::ios_base::openmode mode)
	: Slurper()
{
	m_mode = mode;
	Open(filename, mode);
}
inline Slurper::Slurper(const char* filename, const std::ios_base::openmode mode)
	: Slurper()
{
	m_mode = mode;
	Open(filename, mode);
}

inline Slurper::~Slurper()
{
	Close();
}


inline std::vector<std::string> Slurper::Slurp(const char* filename, const std::ios_base::openmode mode)
{
	Slurper file(filename, mode);
	return file.ReadLines();
}

inline std::vector<std::string> Slurper::Slurp(const std::string_view filename, const std::ios_base::openmode mode)
{
	Slurper file(filename, mode);
	return file.ReadLines();
}

inline bool Lud::Slurper::Open(const std::string_view filename, const std::ios_base::openmode mode)
{
	if (IsOpen())
	{
		Close();
	}
	if (!( mode & std::ios::in ) && !( mode & std::ios::out ))
	{
		throw std::runtime_error("Must use in or out mode");
	}

	m_file.open(std::string(filename), mode);
	return IsOpen();

}
inline bool Lud::Slurper::Open(const char* filename, const std::ios_base::openmode mode)
{
	if (IsOpen())
	{
		Close();
	}
	if (!( mode & std::ios::in ) && !( mode & std::ios::out ))
	{
		throw std::runtime_error("Must use in or out mode");
	}
	m_file.open(filename, mode);
	return IsOpen();

}

inline bool Slurper::IsOpen() const
{
	return m_file.is_open();
}

inline Slurper& Slurper::Reset()
{
	m_file.clear();
	m_file.seekg(0, std::ios::beg);

	return *this;
}

inline Slurper& Slurper::Write(const char* n, std::streampos s)
{
	m_file.write(n, s);
	return *this;
}

inline Slurper& Slurper::WriteLine(const std::string_view line)
{
	m_file << line;
	NewLine();
	return *this;
}

inline Slurper& Slurper::WriteLine(const char* line)
{
	m_file << line;
	NewLine();

	return *this;
}

inline Slurper& Slurper::WriteLines(const std::span<std::string_view> lines)
{
	for (const auto& line : lines)
	{
		WriteLine(line);
	}

	return *this;
}

inline Slurper& Slurper::WriteLines(const std::span<const char*> lines)
{
	for (const auto& line : lines)
	{
		WriteLine(line);
	}

	return *this;
}

inline Slurper& Slurper::SetTabLevel(int level)
{
	m_tab_level = level;
	m_tabs = std::string(m_tab_level, '\t');
	return *this;
}

inline Slurper& Slurper::NewLine()
{
	m_file << '\n' << m_tabs;
	return *this;
}

inline void Slurper::Close()
{
	m_file.close();
}

template<class T> Slurper& Slurper::ReadChunk(T& t)
{
	Read(std::bit_cast<char*>( &t ), sizeof(t));

	return *this;
}

template <typename T>
inline T Slurper::Read()
{
	T dat;
	m_file >> dat;
	return dat;
}

inline std::string Slurper::ReadLine()
{
	std::string line;
	std::getline(m_file, line);

	return line;
}

inline std::vector<std::string> Slurper::ReadLines()
{
	std::vector<std::string> lines;

	for (std::string line; std::getline(m_file, line);)
	{
		lines.emplace_back(line);
	}
	return lines;
}

inline Slurper& Slurper::Read(char* data, size_t size)
{
	m_file.read(data, size);
	return *this;
}

inline std::streampos Slurper::Where()
{
	if (m_mode & std::ios_base::in)
		return m_file.tellg();
	else
		return m_file.tellp();
}

inline Slurper& Slurper::Move(const std::streamoff pos)
{
	if (m_mode & std::ios_base::in)
		m_file.seekg(pos);
	else
		m_file.seekp(pos);
	return *this;
}

inline Slurper& Slurper::Move(const std::streamoff offset, const std::ios_base::seekdir dir)
{
	if (m_mode & std::ios_base::in)
		m_file.seekg(offset, dir);
	else
		m_file.seekp(offset, dir);
	return *this;
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
	if (!( m_mode & std::ios::ate ))
		Move(0, std::ios::end);
	const size_t file_size = static_cast<size_t>( Where() );
	std::vector<T> buffer(file_size / sizeof(T));
	Move(0);
	m_file.read(std::bit_cast<char*>( buffer.data() ), file_size);

	return buffer;
}
template<typename T>
inline std::vector<T> Slurper::SlurpTo(const std::string_view filename, std::ios_base::openmode mode)
{
	Slurper file(filename, mode | std::ios::ate);
	return file.ReadTo<T>();
}

template<typename T>
inline std::vector<T> Slurper::SlurpTo(const char* filename, std::ios_base::openmode mode)
{
	Slurper file(filename, mode | std::ios::ate);
	return file.ReadTo<T>();
}

template <typename T>
inline Slurper& Slurper::WriteList(const std::span<T> list, const std::string_view delim, std::optional<size_t> chunk_size)
{
	for (size_t i = 0; i < list.size(); i++)
	{
		if (chunk_size && i % *chunk_size == 0)
		{
			NewLine();
		}
		m_file << list[i];
		if (i != list.size() - 1)
		{
			m_file << delim;
		}
	}

	return *this;
}

template<typename T>
inline Slurper& Slurper::WriteList(const std::span<T> list, const char* delim, std::optional<size_t> chunk_size)
{
	for (size_t i = 0; i < list.size(); i++)
	{
		if (chunk_size && i % *chunk_size == 0)
		{
			NewLine();
		}
		m_file << list[i];
		if (i != list.size() - 1)
		{
			m_file << delim;
		}
	}
	return *this;
}

template <typename T>
inline Slurper& Slurper::Write(T dat)
{
	m_file << dat;
	return *this;
}
template<class ...Args>
inline Slurper& Slurper::WriteFmt(const std::format_string<Args...> fmt, Args ...args)
{
	m_file << std::format(fmt, std::forward<Args>(args)...);
	return *this;
}
template<typename T>
inline Slurper& Slurper::WriteListFmt(const std::span<T> list, const std::format_string<T> fmt, const std::string_view delim, std::optional<size_t> chunk_size)
{
	for (size_t i = 0; i < list.size(); i++)
	{
		if (chunk_size && i % *chunk_size == 0)
		{
			NewLine();
		}
		m_file << std::format(fmt, std::forward<T>(list[i]));
		if (i != list.size() - 1)
		{
			m_file << delim;
		}
	}
	return *this;
}
template<typename T>
inline Slurper& Slurper::WriteListFmt(const std::span<T> list, const std::format_string<T> fmt, const char* delim, std::optional<size_t> chunk_size)
{
	for (size_t i = 0; i < list.size(); i++)
	{
		if (chunk_size && i % *chunk_size == 0)
		{
			NewLine();
		}
		m_file << std::format(fmt, std::forward<T>(list[i]));
		if (i != list.size() - 1)
		{
			m_file << delim;
		}
	}
	return *this;
}
template<class ...Args>
inline Slurper& Slurper::WriteFmtLine(const std::format_string<Args...> fmt, Args ...args)
{
	m_file << std::format(fmt, std::forward<Args>(args)...);
	NewLine();

	return *this;
}
template<typename T>
inline std::ostream& Slurper::operator<<(const T& t)
{
	m_file << t;
	return m_file;
}
template<typename T>
inline std::istream& Slurper::operator>>(T& t)
{
	m_file >> t;
	return m_file;
}
}

#endif
#endif