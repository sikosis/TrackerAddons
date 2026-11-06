#include "FileNameBuilder.h"

#include <algorithm>
#include <cctype>


namespace {


std::string SanitiseComponent(const std::string& value)
{
	std::string result;
	result.reserve(value.size());
	for (unsigned char character : value) {
		if (character == '/' || character == '\\' || character < 0x20)
			result += ' ';
		else
			result += static_cast<char>(character);
	}

	const std::string::size_type first = result.find_first_not_of(" \t\r\n");
	if (first == std::string::npos)
		return std::string();
	const std::string::size_type last = result.find_last_not_of(" \t\r\n");
	return result.substr(first, last - first + 1);
}
//---------------------------------------------------------------------------------------------------------------------------------//


bool ResolveWildcard(const TagData& data, const std::string& wildcard,
	std::string& value)
{
	if (wildcard == "track")
		value = data.track == 0 ? std::string() : std::to_string(data.track);
	else if (wildcard == "artist")
		value = data.artist;
	else if (wildcard == "title")
		value = data.title;
	else if (wildcard == "album")
		value = data.album;
	else if (wildcard == "year")
		value = data.year;
	else
		return false;
	value = SanitiseComponent(value);
	return true;
}
//---------------------------------------------------------------------------------------------------------------------------------//


bool HasMp3Extension(const std::string& filename)
{
	if (filename.size() < 4)
		return false;
	std::string extension = filename.substr(filename.size() - 4);
	std::transform(extension.begin(), extension.end(), extension.begin(),
		[](unsigned char character) { return std::tolower(character); });
	return extension == ".mp3";
}
//---------------------------------------------------------------------------------------------------------------------------------//


} // namespace


const char* const kDefaultRenamePattern = "{artist} - {title}.mp3";
const char* const kTrackArtistTitlePattern
	= "{track} - {artist} - {title}.mp3";
const char* const kArtistTrackTitlePattern
	= "{artist} - {track} - {title}.mp3";


bool BuildTagFileName(const TagData& data, const std::string& pattern,
	std::string& filename, std::string& error)
{
	if (pattern.empty()) {
		error = "Enter a rename pattern first.";
		return false;
	}

	filename.clear();
	std::string::size_type offset = 0;
	while (offset < pattern.size()) {
		const std::string::size_type opening = pattern.find('{', offset);
		if (opening == std::string::npos) {
			filename += pattern.substr(offset);
			break;
		}
		filename += pattern.substr(offset, opening - offset);
		const std::string::size_type closing = pattern.find('}', opening + 1);
		if (closing == std::string::npos) {
			error = "The rename pattern contains an unmatched '{'.";
			return false;
		}
		const std::string wildcard
			= pattern.substr(opening + 1, closing - opening - 1);
		std::string value;
		if (!ResolveWildcard(data, wildcard, value)) {
			error = "Unknown rename wildcard: {" + wildcard + "}.";
			return false;
		}
		if (value.empty()) {
			error = "The {" + wildcard + "} tag is empty.";
			return false;
		}
		filename += value;
		offset = closing + 1;
	}

	filename = SanitiseComponent(filename);
	if (filename.empty()) {
		error = "The rename pattern produced an empty filename.";
		return false;
	}
	if (!HasMp3Extension(filename))
		filename += ".mp3";
	if (filename.size() > 255) {
		error = "The filename produced by these tags is too long.";
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------------------------------------------------------------//
