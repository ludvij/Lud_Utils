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
#include <assert.h>

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
std::vector<uint8_t> UncompressDeflateStream(const FileInZipData& zipped_file, std::istream& stream);

[[nodiscard]]
std::vector<uint8_t> Compress(const std::span<uint8_t> view);

[[nodiscard]]
std::vector<uint8_t> Decompress(const std::span<uint8_t> view);


}


#include <bit>
#include <cmath>
#include <memory>



#define LUD_UNZIP_READ_BINARY_PTR(stream, ptr, sz) stream.read(std::bit_cast<char*>(ptr), sz)
#define LUD_UNZIP_READ_BINARY(stream, var) LUD_UNZIP_READ_BINARY_PTR(stream, &var, sizeof var)


namespace Lud
{
	
namespace _detail_archive_
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

	static constexpr uint16_t COMPRESSION_DEFLATE = 8;
	static constexpr uint16_t COMPRESSION_NONE = 0;

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
		LUD_UNZIP_READ_BINARY_PTR(stream, buf, file_name_length);
		file_name = std::string(buf, file_name_length);
		delete[] buf;
		
		extra_field = new uint8_t[extra_field_length];
		LUD_UNZIP_READ_BINARY_PTR(stream, extra_field, extra_field_length);
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

		LUD_UNZIP_READ_BINARY_PTR(stream, buf, file_name_length);
		file_name = std::string(buf, file_name_length);

		LUD_UNZIP_READ_BINARY_PTR(stream, extra_field.get(), extra_field_length);

		LUD_UNZIP_READ_BINARY_PTR(stream, buf, file_comment_length);
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

		LUD_UNZIP_READ_BINARY_PTR(stream, buf, comment_length);
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



inline size_t find_eocd_size(std::istream& stream)
{
	// find a signature 0x06054b50
	// advance 16 Bytes to get "offset" to start of central directory (from begginig of file)
	// check if offsset is signature of central directory header 0x02014b50 
	// check that the EOCD ends at EOF
	stream.seekg(0, std::ios::end);
	const size_t file_size = stream.tellg();
	const size_t max_eocd_size = 0xFFFF + _detail_archive_::EndOfCentralDirectoryRecord::BASE_SIZE;
	const size_t search_size = std::min(max_eocd_size, file_size);


	// starts at the end - base EOCD size, iterates until min(max_uint16_t, file_size) + base EOCD size
	const size_t eocd_size  = EndOfCentralDirectoryRecord::BASE_SIZE;
	const size_t loop_end   = file_size - search_size;
	const size_t loop_begin = file_size - eocd_size;

	for (size_t i = loop_begin; i >= loop_end; --i)
	{
		stream.seekg(i);
		int signature;
		LUD_UNZIP_READ_BINARY(stream, signature);
		if (signature == EndOfCentralDirectoryRecord::SIGNATURE)
		{
			stream.seekg(i + EndOfCentralDirectoryRecord::OFFSET_OFFSET);
			uint32_t offset;
			LUD_UNZIP_READ_BINARY(stream, offset);
			stream.seekg(offset);
			uint32_t dir_sig;
			LUD_UNZIP_READ_BINARY(stream, dir_sig);
			if (dir_sig != CentralDirectoryHeader::SIGNATURE)
			{
				continue;
			}
			stream.seekg(i + EndOfCentralDirectoryRecord::COMMENT_L_OFFSET);
			uint16_t comment_sz;
			LUD_UNZIP_READ_BINARY(stream, comment_sz);
			// eocd not at the end of file
			if (i + eocd_size + comment_sz != file_size)
			{
				continue;
			}
			return eocd_size + comment_sz;
		}
	}
	// should never happen
	throw std::runtime_error("unable to find EOCD");
}


// https://stackoverflow.com/a/18704403
// https://stackoverflow.com/a/8662398
// can't use uncomnpress, buwomp
inline size_t uncompress_oneshot(uint8_t* src, uint32_t src_len, uint8_t* dst, uint32_t dst_len)
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

	if (err != Z_OK)
	{
		return err;
	}
	return ret;

}
}

inline std::vector<FileInZipData> CreateZipDirectory(std::istream& stream)
{
	// get buffer containing possible eocd

	const ptrdiff_t eocd_size = _detail_archive_::find_eocd_size(stream);

	// first we need to search for the EOCD
	stream.seekg(-eocd_size, std::ios::end);
	auto eocd = _detail_archive_::EndOfCentralDirectoryRecord(stream);

	// obtain central directory records
	stream.seekg(eocd.offset);
	const int cd_amount = eocd.directory_record_number;
	std::vector<FileInZipData> compressed_files_data;
	compressed_files_data.reserve(cd_amount);

	for (int i = 0; i < cd_amount; i++)
	{
		auto header = _detail_archive_::CentralDirectoryHeader(stream);
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


inline std::vector<uint8_t> UncompressDeflateStream(const FileInZipData& zipped_file, std::istream& stream)
{
	const auto cur_pos = stream.tellg();
	stream.seekg(zipped_file.offset);
	const _detail_archive_::LocalFileHeader lfh(stream);
	std::vector<uint8_t> out_buffer(lfh.uncompressed_size);


	if(lfh.compression_method == _detail_archive_::LocalFileHeader::COMPRESSION_DEFLATE)
	{
		int err = Z_OK;
		uint8_t* in_buffer = new uint8_t[lfh.compressed_size];
		LUD_UNZIP_READ_BINARY_PTR(stream, in_buffer, lfh.compressed_size);
		err = _detail_archive_::uncompress_oneshot(in_buffer, lfh.compressed_size, out_buffer.data(), lfh.uncompressed_size);
		delete[] in_buffer;
		
		switch (err)
		{
		case Z_BUF_ERROR: // dst_len not big enought to hold inflated data
			throw std::runtime_error("dst_len not big enought to hold inflated data");
		case Z_MEM_ERROR: // not enough memory, rip
			throw std::runtime_error("not enough memory, rip");
		case Z_DATA_ERROR: // input data invalid
			throw std::runtime_error("input data invalid or corrupted");
		}
	}
	else if (lfh.compression_method == _detail_archive_::LocalFileHeader::COMPRESSION_NONE)
	{
		LUD_UNZIP_READ_BINARY_PTR(stream, out_buffer.data(), lfh.uncompressed_size);
	}

	
	// uint8_t* out_buffer = new uint8_t[lfh.uncompressed_size];
	
	
	stream.seekg(cur_pos);
	return out_buffer;
}


inline std::vector<uint8_t> Compress(const std::span<uint8_t> view)
{
	int err;
	z_stream strm;

	int flush;

	strm.zalloc = Z_NULL;
	strm.zfree  = Z_NULL;
	strm.opaque = Z_NULL;

	size_t current_chunks = 0;

	std::vector<uint8_t> deflated_stream;

	constexpr size_t CHUNK = 16384;
	uint8_t out[CHUNK];

	err = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
	if (err != Z_OK)
	{
		throw std::runtime_error(std::format("ZLIB ERROR: {}", zError(err)));
	}
	// compress unitl in buffer is exhausted
	do {
		// flush stuff
		strm.avail_in = CHUNK * (current_chunks + 1) > view.size() ? view.size() - CHUNK * current_chunks : CHUNK;
		strm.next_in = view.data() + CHUNK * current_chunks;
		current_chunks++;
		flush = strm.avail_in == CHUNK ? Z_NO_FLUSH : Z_FINISH;
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			err = deflate(&strm, flush);
			if (err < Z_OK)
			{
				deflateEnd(&strm);
				throw std::runtime_error(std::format("ZLIB ERROR: {}", zError(err)));
			}
			const size_t have = CHUNK - strm.avail_out;
			deflated_stream.insert(deflated_stream.end(), out, out + have);
		} while(strm.avail_out == 0);
		assert(strm.avail_in == 0);

	} while(flush != Z_FINISH);
	deflateEnd(&strm);
	if (err != Z_STREAM_END)
	{
		throw std::runtime_error("ZLIB ERROR: Stream could not finish");
	}

	return deflated_stream;
}

inline std::vector<uint8_t> Decompress(const std::span<uint8_t> view)
{
	int err;
	z_stream strm;


	strm.zalloc = Z_NULL;
	strm.zfree  = Z_NULL;
	strm.opaque = Z_NULL;

	strm.avail_in = view.size();
	strm.total_in = view.size();
	strm.next_in  = view.data();


	std::vector<uint8_t> inflated_stream;

	constexpr size_t CHUNK = 16384;
	uint8_t out[CHUNK];

	err = inflateInit(&strm);
	if (err != Z_OK)
	{
		throw std::runtime_error(std::format("ZLIB ERROR: {}", zError(err)));
	}

	do {
		strm.avail_out = CHUNK;
		strm.next_out = out;
		err = inflate(&strm, Z_NO_FLUSH);
		
		switch (err)
		{
		case Z_NEED_DICT:
			err = Z_DATA_ERROR;
			[[fallthrough]];
		case Z_DATA_ERROR: 
			[[fallthrough]];
		case Z_MEM_ERROR:
			inflateEnd(&strm);
			throw std::runtime_error("ZLIB MEMORY ERROR");
		}
		const size_t have = CHUNK - strm.avail_out;
		inflated_stream.insert(inflated_stream.end(), out, out + have);

	} while(err != Z_STREAM_END);

	inflateEnd(&strm);
	if (err != Z_STREAM_END)
	{
		throw std::runtime_error("ZLIB ERROR: Stream could not finish");
	}

	return inflated_stream;
}
}

#endif // !LUD_ARCHIVE_HEADER