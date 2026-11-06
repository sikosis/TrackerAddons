#ifndef TAG_LIB_TAG_H
#define TAG_LIB_TAG_H

#include "TagData.h"

#include <string>


class TagLibTag {
public:
	explicit TagLibTag(const std::string& path);

	bool Load(TagData& data, std::string& error);
	bool Save(const TagData& data, std::string& error);

private:
	std::string fPath;
};


#endif
