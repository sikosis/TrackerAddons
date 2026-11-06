#ifndef ID3V1_TAG_H
#define ID3V1_TAG_H

#include "TagData.h"

#include <string>


class Id3v1Tag {
public:
	explicit Id3v1Tag(const std::string& path);

	bool Load(TagData& data, std::string& error);
	bool Save(const TagData& data, std::string& error);

private:
	std::string fPath;
	bool fHasTag;
};


#endif
