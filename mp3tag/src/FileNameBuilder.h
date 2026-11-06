#ifndef FILE_NAME_BUILDER_H
#define FILE_NAME_BUILDER_H

#include "TagData.h"

#include <string>


extern const char* const kDefaultRenamePattern;
extern const char* const kTrackArtistTitlePattern;
extern const char* const kArtistTrackTitlePattern;

bool BuildTagFileName(const TagData& data, const std::string& pattern,
	std::string& filename, std::string& error);


#endif
