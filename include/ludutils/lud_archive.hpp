#ifndef LUD_ARCHIVE_HEADER
#define LUD_ARCHIVE_HEADER

/**
 * REQURIES ZLIB TO WORK
 *
 * ZIP64 not supported right now
 * CRC-32 not supported right now
 * Encryption not supported right now
 * File attributes not supported right now
 *
 * this only extracts the file raw data, but the metadata is largely ignored
 * should be a fun exercise to figure that out
 */

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <streambuf>
#include <vector>
#include "zlib/zlib.h"

namespace Lud
{
struct FileInZipData
{
	std::string file_name;
	uint32_t offset;
	uint32_t uncompressed_size;
	uint32_t compressed_size;
	uint32_t CRC_32;
};

[[nodiscard]]
std::vector<FileInZipData> CreateZipDirectory(std::istream& stream);

[[nodiscard]]
std::vector<uint8_t> UncompressDeflateStream(FileInZipData& zipped_file, std::istream& stream);


}


#include <bit>
#include <cmath>
#include <memory>



#define LUD_UNZIP_READ_BINARY_PTR(stream, ptr, sz) stream.read(std::bit_cast<char*>(ptr), sz)
#define LUD_UNZIP_READ_BINARY(stream, var) LUD_READ_BINARY_PTR(stream, &var, sizeof var)


namespace Lud
{
	
namespace _detail_str_
{
// https://pkwaredownloads.blob.core.windows.net/pkware-general/Documentation/APPNOTE-6.3.9.TXT
struct LocalFileHeader
{
	uint32_t signature{};
	uint16_t version{};
	uint16_t gen_purpose_flag{};
	uint16_t compression_method{};
	uint16_t file_last_modification_time{};
	uint16_t file_last_modification_date{};
	uint32_t CRC_32{};
	uint32_t compressed_size{};
	uint32_t uncompressed_size{};
	uint16_t file_name_length{};
	uint16_t extra_field_length{};

	std::string file_name{};
	// made a ptr so it can be used with memory_istream
	// maybe looking for better solutions
	uint8_t* extra_field{};

	LocalFileHeader(const LocalFileHeader& other) = delete;
	LocalFileHeader(LocalFileHeader&& other) = delete;
	LocalFileHeader& operator=(const LocalFileHeader& other) = delete;
	LocalFileHeader& operator=(LocalFileHeader&& other) = delete;

	explicit LocalFileHeader(std::istream& stream)
	{
		LUD_UNZIP_READ_BINARY(stream, signature);
		LUD_UNZIP_READ_BINARY(stream, version);
		LUD_UNZIP_READ_BINARY(stream, gen_purpose_flag);
		LUD_UNZIP_READ_BINARY(stream, compression_method);
		LUD_UNZIP_READ_BINARY(stream, file_last_modification_time);
		LUD_UNZIP_READ_BINARY(stream, file_last_modification_date);
		LUD_UNZIP_READ_BINARY(stream, CRC_32);
		LUD_UNZIP_READ_BINARY(stream, compressed_size);
		LUD_UNZIP_READ_BINARY(stream, uncompressed_size);
		LUD_UNZIP_READ_BINARY(stream, file_name_length);
		LUD_UNZIP_READ_BINARY(stream, extra_field_length);

		
		char* buf = new char[file_name_length];
		LUD_READ_BINARY_PTR(stream, buf, file_name_length);
		file_name = std::string(buf, file_name_length);
		delete[] buf;
		
		extra_field = new uint8_t[extra_field_length];
		LUD_READ_BINARY_PTR(stream, extra_field, extra_field_length);
	}

	~LocalFileHeader()
	{
		delete[] extra_field;
	}
};

struct CentralDirectoryHeader
{
	uint32_t signature{};
	uint16_t version_made_by{};
	uint16_t version_to_extract{};
	uint16_t gen_purpose_flag{};
	uint16_t compression_method{};
	uint16_t file_last_modification_time{};
	uint16_t file_last_modification_date{};
	uint32_t CRC_32{};
	uint32_t compressed_size{};
	uint32_t uncompressed_size{};
	uint16_t file_name_length{};
	uint16_t extra_field_length{};
	uint16_t file_comment_length{};
	uint16_t disk_start{};
	uint16_t internal_file_attrib{};
	uint32_t external_file_attrib{};
	uint32_t offset{};

	std::string file_name{};
	std::shared_ptr<uint8_t[]> extra_field{};
	std::string file_comment{};

	static constexpr uint32_t SIGNATURE = 0x02014b50;

	explicit CentralDirectoryHeader(std::istream& stream)
	{
		LUD_UNZIP_READ_BINARY(stream, signature);
		LUD_UNZIP_READ_BINARY(stream, version_made_by);
		LUD_UNZIP_READ_BINARY(stream, version_to_extract);
		LUD_UNZIP_READ_BINARY(stream, gen_purpose_flag);
		LUD_UNZIP_READ_BINARY(stream, compression_method);
		LUD_UNZIP_READ_BINARY(stream, file_last_modification_time);
		LUD_UNZIP_READ_BINARY(stream, file_last_modification_date);
		LUD_UNZIP_READ_BINARY(stream, CRC_32);
		LUD_UNZIP_READ_BINARY(stream, compressed_size);
		LUD_UNZIP_READ_BINARY(stream, uncompressed_size);
		LUD_UNZIP_READ_BINARY(stream, file_name_length);
		LUD_UNZIP_READ_BINARY(stream, extra_field_length);
		LUD_UNZIP_READ_BINARY(stream, file_comment_length);
		LUD_UNZIP_READ_BINARY(stream, disk_start);
		LUD_UNZIP_READ_BINARY(stream, internal_file_attrib);
		LUD_UNZIP_READ_BINARY(stream, external_file_attrib);
		LUD_UNZIP_READ_BINARY(stream, offset);

		extra_field  = std::make_shared<uint8_t[]>(extra_field_length);

		const auto sz = std::max({ file_name_length, file_comment_length });
		char* buf = new char[sz];

		LUD_READ_BINARY_PTR(stream, buf, file_name_length);
		file_name = std::string(buf, file_name_length);

		LUD_READ_BINARY_PTR(stream, extra_field.get(), extra_field_length);

		LUD_READ_BINARY_PTR(stream, buf, file_comment_length);
		file_comment = std::string(buf, file_comment_length);

		delete[] buf;
	}
};

struct EndOfCentralDirectoryRecord
{
	uint32_t signature{};
	uint16_t disk_number{};
	uint16_t disk_start_number{};
	uint16_t directory_record_number{};
	uint16_t directory_record_number_disk{};
	uint32_t central_directory_size{};
	uint32_t offset{};
	uint16_t comment_length{};
	std::string comment{};

	static constexpr uint32_t SIGNATURE = 0x06054b50;
	static constexpr size_t OFFSET_OFFSET = 16;
	static constexpr size_t COMMENT_L_OFFSET = 20;
	static constexpr size_t BASE_SIZE = 22;

	explicit EndOfCentralDirectoryRecord(std::istream& stream)
	{
		LUD_UNZIP_READ_BINARY(stream, signature);
		LUD_UNZIP_READ_BINARY(stream, disk_number);
		LUD_UNZIP_READ_BINARY(stream, disk_start_number);
		LUD_UNZIP_READ_BINARY(stream, directory_record_number);
		LUD_UNZIP_READ_BINARY(stream, directory_record_number_disk);
		LUD_UNZIP_READ_BINARY(stream, central_directory_size);
		LUD_UNZIP_READ_BINARY(stream, offset);
		LUD_UNZIP_READ_BINARY(stream, comment_length);

		char* buf = new char[comment_length];

		LUD_READ_BINARY_PTR(stream, buf, comment_length);
		comment = std::string(buf, comment_length);

		delete[] buf;
	}

};

struct DataDescriptor
{
	uint32_t optional_signature{};
	uint32_t CRC_32{};
	uint32_t compressed_size{};
	uint32_t uncompressed_size{};

	static constexpr uint32_t SIGNATURE = 0x08074b50;

	DataDescriptor() = default;

	explicit DataDescriptor(std::istream& stream)
	{
		LUD_UNZIP_READ_BINARY(stream, CRC_32);
		if (CRC_32 == SIGNATURE)
		{
			optional_signature = CRC_32;
			LUD_UNZIP_READ_BINARY(stream, CRC_32);
		}
		LUD_UNZIP_READ_BINARY(stream, compressed_size);
		LUD_UNZIP_READ_BINARY(stream, uncompressed_size);
	}
};


// meant for LE, no size checking since this is only used in the context of find_eocd_size
constexpr uint32_t u8x4_to_u32(const uint8_t* ptr)
{
	// return *std::bit_cast<uint32_t*>(ptr);
	return ptr[0] | ptr[1] << 8 | ptr[2] << 16 | ptr[3] << 24;
}

// meant for LE, no size checking since this is only used in the context of find_eocd_size
constexpr uint32_t u8x2_to_u16(const uint8_t* ptr)
{
	// return *std::bit_cast<uint16_t*>(ptr);
	return ptr[0] | ptr[1] << 8;
}


//- TODO: make this work with an istream in order to make it work with files too
// TODO: investigate if making a buffer is better than using the stream for searching
size_t find_eocd_size(std::istream& stream)
{
	// find a signature 0x06054b50
	// advance 16 Bytes to get "offset" to start of central directory (from begginig of file)
	// check if offsset is signature of central directory header 0x02014b50 
	// check that the EOCD ends at EOF
	stream.seekg(0, std::ios::end);
	const size_t file_size = stream.tellg();
	const size_t max_eocd_size = 0xFFFF + _detail_str_::EndOfCentralDirectoryRecord::BASE_SIZE;
	const size_t buf_size = std::min(max_eocd_size, file_size);

	const uint8_t* buf = new uint8_t[buf_size];
	stream.seekg(-buf_size, std::ios::end);
	LUD_READ_BINARY_PTR(stream, buf, buf_size);

	// starts at the end - base EOCD size, iterates until min(max_uint16_t, file_size) + base EOCD size
	const size_t eocd_size  = EndOfCentralDirectoryRecord::BASE_SIZE;
	const size_t loop_end   = 0;
	const size_t loop_begin = buf_size - eocd_size;

	for (auto i = loop_begin; i >= loop_end; --i)
	{
		if (buf[i] == 0x50 && buf[i + 1] == 0x4b)
		{

			const uint32_t sig = u8x4_to_u32(buf + i);
			if (sig != EndOfCentralDirectoryRecord::SIGNATURE)
			{
				continue;
			}
			const auto offset = u8x4_to_u32(buf + i + EndOfCentralDirectoryRecord::OFFSET_OFFSET);
			stream.seekg(offset, std::ios::beg);
			uint32_t dir_sig;
			LUD_UNZIP_READ_BINARY(stream, dir_sig);
			if (dir_sig != CentralDirectoryHeader::SIGNATURE)
			{
				continue;
			}
			const auto comment_sz = u8x2_to_u16(buf + i + EndOfCentralDirectoryRecord::COMMENT_L_OFFSET);
			// eocd not at the end of file
			if (i + eocd_size + comment_sz != buf_size)
			{
				continue;
			}
			delete[] buf;
			return eocd_size + comment_sz;
		}
	}
	// should never happen
	delete[] buf;
	throw std::runtime_error("unable to find EOCD");
}


// https://stackoverflow.com/a/18704403
// https://stackoverflow.com/a/8662398
// can't use uncomnpress, buwomp
size_t uncompress_oneshot(uint8_t* src, uint32_t src_len, uint8_t* dst, uint32_t dst_len)
{
	z_stream strm = {};
	
	strm.total_in  = src_len;
	strm.avail_in  = src_len;
	strm.total_out = dst_len;
	strm.avail_out = dst_len;
	
	strm.next_in  = src;
	strm.next_out = dst;

	strm.zalloc = Z_NULL;
	strm.zfree  = Z_NULL;
	strm.opaque = Z_NULL;

	int err = -1;
	size_t ret;
	err = inflateInit2(&strm, -MAX_WBITS);
	if (err == Z_OK)
	{
		err = inflate(&strm, Z_FINISH);
		if (err == Z_STREAM_END)
		{
			ret = strm.total_out;
		}
	}
	inflateEnd(&strm);

	switch (err)
	{
	case Z_BUF_ERROR: // dst_len not big enought to hold inflated data
		throw std::runtime_error("dst_len not big enought to hold inflated data");
	case Z_MEM_ERROR: // not enough memory, rip
		throw std::runtime_error("not enough memory, rip");
	case Z_DATA_ERROR: // input data invalid
		throw std::runtime_error("input data invalid or corrupted");
	}
	return ret;

}
}

inline std::vector<FileInZipData> CreateZipDirectory(std::istream& stream)
{
	// get buffer containing possible eocd

	const ptrdiff_t eocd_size = _detail_str_::find_eocd_size(stream);

	// first we need to search for the EOCD
	stream.seekg(-eocd_size, std::ios::end);
	auto eocd = _detail_str_::EndOfCentralDirectoryRecord(stream);

	// obtain central directory records
	stream.seekg(eocd.offset);
	const int cd_amount = eocd.directory_record_number;
	std::vector<FileInZipData> compressed_files_data;
	compressed_files_data.reserve(cd_amount);

	for (int i = 0; i < cd_amount; i++)
	{
		auto header = _detail_str_::CentralDirectoryHeader(stream);
		if (header.uncompressed_size == 0)
		{
			continue;
		}
		compressed_files_data.emplace_back(
			header.file_name,
			header.offset,
			header.uncompressed_size,
			header.compressed_size,
			header.CRC_32
		);

	}
	return compressed_files_data;
}


inline std::vector<uint8_t> UncompressDeflateStream(FileInZipData& zipped_file, std::istream& stream)
{
	const auto cur_pos = stream.tellg();
	stream.seekg(zipped_file.offset);
	const _detail_str_::LocalFileHeader lfh(stream);

	uint8_t* in_buffer = new uint8_t[lfh.compressed_size];
	LUD_READ_BINARY_PTR(stream, in_buffer, lfh.compressed_size);

	// uint8_t* out_buffer = new uint8_t[lfh.uncompressed_size];
	std::vector<uint8_t> out_buffer(lfh.uncompressed_size);
	_detail_str_::uncompress_oneshot(in_buffer, lfh.compressed_size, out_buffer.data(), lfh.uncompressed_size);

	
	delete[] in_buffer;
	stream.seekg(cur_pos);
	return out_buffer;
}

}

#endif // !LUD_ARCHIVE_HEADER
