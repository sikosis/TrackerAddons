#ifndef FILE_NAME_BUILDER_H
#define FILE_NAME_BUILDER_H

#include "TagData.h"

#include <string>


bool BuildTagFileName(const TagData& data, bool includeTrackNumber,
	std::string& filename, std::string& error);


#endif
