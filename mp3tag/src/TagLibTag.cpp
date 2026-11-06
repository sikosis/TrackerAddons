#include "TagLibTag.h"

#include "Genres.h"

#include <fileref.h>
#include <tag.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>


namespace {


std::string ToUtf8(const TagLib::String& value)
{
	return value.isEmpty() ? std::string() : value.to8Bit(true);
}
//---------------------------------------------------------------------------------------------------------------------------------//


TagLib::String FromUtf8(const std::string& value)
{
	return TagLib::String(value, TagLib::String::UTF8);
}
//---------------------------------------------------------------------------------------------------------------------------------//


std::string Lowercase(const std::string& value)
{
	std::string result = value;
	std::transform(result.begin(), result.end(), result.begin(),
		[](unsigned char character) { return std::tolower(character); });
	return result;
}
//---------------------------------------------------------------------------------------------------------------------------------//


std::uint8_t GenreIndex(const TagLib::String& genre)
{
	const std::string name = Lowercase(ToUtf8(genre));
	for (std::size_t index = 0; index < kGenreCount; ++index) {
		if (Lowercase(kGenreNames[index]) == name)
			return static_cast<std::uint8_t>(index);
	}
	return 255;
}
//---------------------------------------------------------------------------------------------------------------------------------//


unsigned int ParseYear(const std::string& value)
{
	if (value.empty())
		return 0;
	char* end = NULL;
	const unsigned long year = std::strtoul(value.c_str(), &end, 10);
	if (end == value.c_str() || *end != '\0' || year > 9999)
		return 0;
	return static_cast<unsigned int>(year);
}
//---------------------------------------------------------------------------------------------------------------------------------//


} // namespace


TagLibTag::TagLibTag(const std::string& path)
	:
	fPath(path)
{
}
//---------------------------------------------------------------------------------------------------------------------------------//


bool TagLibTag::Load(TagData& data, std::string& error)
{
	TagLib::FileRef file(fPath.c_str());
	if (file.isNull() || file.tag() == NULL) {
		error = "TagLib could not read metadata from the MP3 file.";
		return false;
	}

	TagLib::Tag* tag = file.tag();
	data.title = ToUtf8(tag->title());
	data.artist = ToUtf8(tag->artist());
	data.album = ToUtf8(tag->album());
	data.year = tag->year() == 0 ? std::string() : std::to_string(tag->year());
	data.comment = ToUtf8(tag->comment());
	data.track = tag->track();
	data.genre = GenreIndex(tag->genre());
	return true;
}
//---------------------------------------------------------------------------------------------------------------------------------//


bool TagLibTag::Save(const TagData& data, std::string& error)
{
	TagLib::FileRef file(fPath.c_str());
	if (file.isNull() || file.tag() == NULL) {
		error = "TagLib could not open the MP3 metadata for writing.";
		return false;
	}

	TagLib::Tag* tag = file.tag();
	tag->setTitle(FromUtf8(data.title));
	tag->setArtist(FromUtf8(data.artist));
	tag->setAlbum(FromUtf8(data.album));
	tag->setYear(ParseYear(data.year));
	tag->setComment(FromUtf8(data.comment));
	tag->setTrack(data.track);
	if (data.genre < kGenreCount)
		tag->setGenre(FromUtf8(kGenreNames[data.genre]));
	else
		tag->setGenre(TagLib::String());

	if (!file.save()) {
		error = "TagLib could not save the MP3 metadata.";
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------------------------------------------------------------//
