#ifndef EDITOR_WINDOWS_H
#define EDITOR_WINDOWS_H

#include "TagData.h"

#include <SupportDefs.h>

#include <string>


struct BatchOptions {
	bool usePattern = false;
	std::string pattern = "/a - /t.mp3";
	bool setTitle = false;
	bool setArtist = false;
	bool setAlbum = false;
	bool setYear = false;
	bool setComment = false;
	bool setGenre = false;
	TagData data;
};


bool EditSingleTag(const char* filename, TagData& data);
bool EditMultipleTags(BatchOptions& options);


#endif
