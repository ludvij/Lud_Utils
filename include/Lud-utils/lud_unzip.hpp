#ifndef LUD_UNZIP_HEADER
#define LUD_UNZIP_HEADER
/**
* REQURIES ZLIB TO WORK
*/

#include <cstdint>
#include <string_view>

namespace Lud
{
struct LocalFileHeader
{
	char signature[4];
	uint16_t version;
	uint16_t gen_purpose_flag;
	uint16_t compression_method;
	uint16_t file_last_modification_time;
	uint16_t file_last_modification_date;
	uint32_t CRC32_uncompress_data;
	uint32_t compressed_size;
	uint32_t uncompressed_size;
	uint16_t file_name_length;
	uint16_t extra_field_length;
	char* file_name;
	char* extra_field;

	LocalFileHeader(std::string_view dat);
	~LocalFileHeader();
};
}

#ifdef LUD_UNZIP_HEADER




Lud::LocalFileHeader::LocalFileHeader(std::string_view dat)
{
}

Lud::LocalFileHeader::~LocalFileHeader()
{
	delete[] file_name;
	delete[] extra_field;
}

#endif // LUD_UNZIP_HEADER
#endif // !LUD_UNZIP_HEADER
