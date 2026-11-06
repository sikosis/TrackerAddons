#include "PatternMatcher.h"

#include <algorithm>
#include <cstddef>


namespace {


std::size_t MaximumLength(char token)
{
	return token == 'y' ? 4 : 30;
}
//---------------------------------------------------------------------------------------------------------------------------------//


bool StoreCapture(char token, const std::string& value, PatternResult& result)
{
	const std::string clipped = value.substr(0, MaximumLength(token));
	switch (token) {
		case 'a':
			result.data.artist = clipped;
			result.hasArtist = true;
			return true;
		case 't':
			result.data.title = clipped;
			result.hasTitle = true;
			return true;
		case 'n':
			result.data.album = clipped;
			result.hasAlbum = true;
			return true;
		case 'y':
			result.data.year = clipped;
			result.hasYear = true;
			return true;
		case 'c':
			result.data.comment = clipped;
			result.hasComment = true;
			return true;
		default:
			return false;
	}
}
//---------------------------------------------------------------------------------------------------------------------------------//


bool MatchFrom(const std::string& filename, std::size_t filenameOffset,
	const std::string& pattern, std::size_t patternOffset,
	PatternResult& result)
{
	while (patternOffset < pattern.size()) {
		if (pattern[patternOffset] != '/') {
			if (filenameOffset >= filename.size()
				|| filename[filenameOffset] != pattern[patternOffset]) {
				return false;
			}
			++filenameOffset;
			++patternOffset;
			continue;
		}

		if (++patternOffset >= pattern.size())
			return false;
		const char token = pattern[patternOffset++];
		if (token != '*' && token != 'a' && token != 't' && token != 'n'
			&& token != 'y' && token != 'c') {
			return false;
		}

		for (std::size_t end = filenameOffset; end <= filename.size(); ++end) {
			PatternResult candidate = result;
			if (token != '*'
				&& !StoreCapture(token,
					filename.substr(filenameOffset, end - filenameOffset), candidate)) {
				return false;
			}
			if (MatchFrom(filename, end, pattern, patternOffset, candidate)) {
				result = candidate;
				return true;
			}
		}
		return false;
	}

	return filenameOffset == filename.size();
}
//---------------------------------------------------------------------------------------------------------------------------------//


} // namespace


bool MatchFilenamePattern(const std::string& filename,
	const std::string& pattern, PatternResult& result)
{
	PatternResult candidate;
	if (!MatchFrom(filename, 0, pattern, 0, candidate))
		return false;
	result = candidate;
	return true;
}
//---------------------------------------------------------------------------------------------------------------------------------//
