#ifndef PATTERN_MATCHER_H
#define PATTERN_MATCHER_H

#include "TagData.h"

#include <string>


struct PatternResult {
	TagData data;
	bool hasTitle = false;
	bool hasArtist = false;
	bool hasAlbum = false;
	bool hasYear = false;
	bool hasComment = false;
};


bool MatchFilenamePattern(const std::string& filename,
	const std::string& pattern, PatternResult& result);


#endif
