#include "FileNameBuilder.h"

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


} // namespace


bool BuildTagFileName(const TagData& data, bool includeTrackNumber,
	std::string& filename, std::string& error)
{
	const std::string artist = SanitiseComponent(data.artist);
	const std::string title = SanitiseComponent(data.title);
	if (artist.empty()) {
		error = "Artist is required before the file can be renamed.";
		return false;
	}
	if (title.empty()) {
		error = "Title is required before the file can be renamed.";
		return false;
	}

	if (includeTrackNumber && data.track == 0) {
		error = "A track number is required when including it in the filename.";
		return false;
	}

	filename = artist + " - " + title + ".mp3";
	if (includeTrackNumber)
		filename = std::to_string(data.track) + " - " + filename;
	if (filename.size() > 255) {
		error = "The filename produced by these tags is too long.";
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------------------------------------------------------------//
