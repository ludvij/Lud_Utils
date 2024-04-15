#ifndef LUD_SLURPER_HEADER
#define LUD_SLURPER_HEADER

#include <ostream>
#include <string_view>
#include <string>

#include <fstream>
#include <filesystem>

#include <vector>
#include <bit>



namespace Lud {

class Slurper 
{
public:
	Slurper();
	Slurper(const std::string_view& filename, std::ios_base::openmode mode = std::ios_base::in);
	Slurper(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in);
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


	// returns current pos in file
	size_t Where();
	// moves current file pos to provided pos
	Slurper& Move(std::streamoff pos);
	// moves current file pos by an offset in a direction
	Slurper& Move(std::streamoff offset, std::ios_base::seekdir dir);

	// returns file pointer to pos 0
	Slurper& Reset();

	// returns true if file was opened
	bool IsOpen() const;

	// opens file 
	Slurper& Open(const std::string_view& filename, std::ios_base::openmode mode = std::ios_base::in);
	Slurper& Open(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in);
	Slurper& Open(const char* filename, std::ios_base::openmode mode = std::ios_base::in);

	// returns file as vector of lines without you needing to creating a slurper
	static std::vector<std::string> Slurp(const std::string_view& filename, std::ios_base::openmode mode = std::ios_base::in);
	static std::vector<std::string> Slurp(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in);
	static std::vector<std::string> Slurp(const char* filename, std::ios_base::openmode mode = std::ios_base::in);

	// reads whole file to T vector
	template<typename T> std::vector<T> ReadTo();

	template<typename T> static std::vector<T> SlurpTo(const std::string_view& filename, std::ios_base::openmode mode = std::ios_base::in);
	template<typename T> static std::vector<T> SlurpTo(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in);
	template<typename T> static std::vector<T> SlurpTo(const char* filename, std::ios_base::openmode mode = std::ios_base::in);

	// closes file
	void Close();

	bool HasSpace(size_t size);

private:
	std::ifstream m_file;
	std::ios_base::openmode m_mode{};

};
}

#ifdef LUD_SLURPER_IMPLEMENTATION
namespace Lud {


inline Slurper::Slurper()
{

}

inline Slurper::Slurper(const std::string_view& filename, const std::ios_base::openmode mode) 
	: Slurper()
{ 
	m_mode = mode;
	Open(filename, mode); 
}
inline Slurper::Slurper(const std::string& filename, const std::ios_base::openmode mode) 
	: Slurper()
{ 
	m_mode = mode;
	Open(filename, mode); 
}
inline Slurper::Slurper(const char* filename, const std::ios_base::openmode mode ) 
	: Slurper()
{ 
	m_mode = mode;
	Open(filename, mode); 
}

inline Slurper::~Slurper()
{
	Close();
}

inline std::vector<std::string> Slurper::Slurp(const std::string& filename, const std::ios_base::openmode mode)
{
	Slurper file(filename, mode);
	return file.ReadLines();
}

inline std::vector<std::string> Slurper::Slurp(const char* filename, const std::ios_base::openmode mode)
{
	Slurper file(filename, mode);
	return file.ReadLines();
}

inline std::vector<std::string> Slurper::Slurp(const std::string_view& filename, const std::ios_base::openmode mode)
{
	Slurper file(filename, mode);
	return file.ReadLines();
}

inline Slurper& Lud::Slurper::Open(const std::string_view& filename, const std::ios_base::openmode mode) 
{
	if (IsOpen()) {
		Close();
	}
	m_file.open(std::string(filename), mode);
	return *this;
}
inline Slurper& Lud::Slurper::Open(const std::string& filename, const std::ios_base::openmode mode) 
{ 
	if (IsOpen()) {
		Close();
	}
	m_file.open(filename, mode);
	return *this;
} 
inline Slurper& Lud::Slurper::Open(const char* filename, const std::ios_base::openmode mode) 
{ 
	if (IsOpen()) {
		Close();
	}
	m_file.open(filename, mode);
	return *this;
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


inline void Slurper::Close()
{
	m_file.close();
}

template<class T> Slurper& Slurper::ReadChunk(T& t)
{
	Read(std::bit_cast<char*>(&t), sizeof(t));

	return *this;
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

	for(std::string line; std::getline(m_file, line);) {
		lines.emplace_back(line);
	}
	return lines;
}

inline Slurper &Slurper::Read(char *data, size_t size)
{
	m_file.read(data, size);
}

inline size_t Slurper::Where()
{
	return m_file.tellg();
}

inline Slurper& Slurper::Move(const std::streamoff pos)
{
	m_file.seekg(pos);
	return *this;
}

inline Slurper& Slurper::Move(const std::streamoff offset, const std::ios_base::seekdir dir)
{
	m_file.seekg(offset, dir);
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
	if (!(m_mode & std::ios::ate))
		Move(0, std::ios::end);
	const size_t file_size = static_cast<size_t>(Where());
	std::vector<T> buffer(file_size / sizeof(T));
	Move(0);
	m_file.read(std::bit_cast<char*>(buffer.data()), file_size);
	
	return buffer;
}
template<typename T>
inline std::vector<T> Slurper::SlurpTo(const std::string_view& filename, std::ios_base::openmode mode)
{
	Slurper file(filename, mode | std::ios::ate);
	return file.ReadTo<T>();
}

template<typename T>
inline std::vector<T> Slurper::SlurpTo(const std::string& filename, std::ios_base::openmode mode)
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

}

#endif
#endif