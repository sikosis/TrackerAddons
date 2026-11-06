#include "Id3v1Tag.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <fstream>


namespace {


struct RawTag {
	char header[3];
	char title[30];
	char artist[30];
	char album[30];
	char year[4];
	char comment[30];
	std::uint8_t genre;
};


static_assert(sizeof(RawTag) == 128, "ID3v1 tags must contain 128 bytes");


std::string DecodeField(const char* bytes, std::size_t size)
{
	const char* terminator = static_cast<const char*>(std::memchr(bytes, '\0', size));
	std::size_t length = terminator == NULL
		? size : static_cast<std::size_t>(terminator - bytes);
	while (length > 0 && (bytes[length - 1] == ' ' || bytes[length - 1] == '\0'))
		--length;
	return std::string(bytes, length);
}
//---------------------------------------------------------------------------------------------------------------------------------//


void EncodeField(const std::string& value, char* bytes, std::size_t size)
{
	std::memset(bytes, ' ', size);
	const std::size_t length = std::min(value.size(), size);
	std::memcpy(bytes, value.data(), length);
}
//---------------------------------------------------------------------------------------------------------------------------------//


} // namespace


Id3v1Tag::Id3v1Tag(const std::string& path)
	:
	fPath(path),
	fHasTag(false)
{
}
//---------------------------------------------------------------------------------------------------------------------------------//


bool Id3v1Tag::Load(TagData& data, std::string& error)
{
	std::ifstream file(fPath, std::ios::binary);
	if (!file) {
		error = "Could not open the file for reading.";
		return false;
	}

	file.seekg(0, std::ios::end);
	const std::streamoff size = file.tellg();
	if (size < 0) {
		error = "Could not determine the file size.";
		return false;
	}

	data = TagData();
	if (size < static_cast<std::streamoff>(sizeof(RawTag))) {
		fHasTag = false;
		return true;
	}

	RawTag tag = {};
	file.seekg(-static_cast<std::streamoff>(sizeof(RawTag)), std::ios::end);
	file.read(reinterpret_cast<char*>(&tag), sizeof(tag));
	if (!file) {
		error = "Could not read the ID3v1 tag.";
		return false;
	}

	fHasTag = std::memcmp(tag.header, "TAG", 3) == 0;
	if (!fHasTag)
		return true;

	data.title = DecodeField(tag.title, sizeof(tag.title));
	data.artist = DecodeField(tag.artist, sizeof(tag.artist));
	data.album = DecodeField(tag.album, sizeof(tag.album));
	data.year = DecodeField(tag.year, sizeof(tag.year));
	data.comment = DecodeField(tag.comment, sizeof(tag.comment));
	data.genre = tag.genre;
	return true;
}
//---------------------------------------------------------------------------------------------------------------------------------//


bool Id3v1Tag::Save(const TagData& data, std::string& error)
{
	std::fstream file(fPath, std::ios::in | std::ios::out | std::ios::binary);
	if (!file) {
		error = "Could not open the file for writing.";
		return false;
	}

	RawTag tag = {};
	std::memcpy(tag.header, "TAG", 3);
	EncodeField(data.title, tag.title, sizeof(tag.title));
	EncodeField(data.artist, tag.artist, sizeof(tag.artist));
	EncodeField(data.album, tag.album, sizeof(tag.album));
	EncodeField(data.year, tag.year, sizeof(tag.year));
	EncodeField(data.comment, tag.comment, sizeof(tag.comment));
	tag.genre = data.genre;

	if (fHasTag)
		file.seekp(-static_cast<std::streamoff>(sizeof(RawTag)), std::ios::end);
	else
		file.seekp(0, std::ios::end);
	file.write(reinterpret_cast<const char*>(&tag), sizeof(tag));
	file.flush();
	if (!file) {
		error = "Could not write the ID3v1 tag.";
		return false;
	}

	fHasTag = true;
	return true;
}
//---------------------------------------------------------------------------------------------------------------------------------//
