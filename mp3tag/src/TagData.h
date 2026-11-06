#ifndef TAG_DATA_H
#define TAG_DATA_H

#include <cstdint>
#include <string>


struct TagData {
	std::string title;
	std::string artist;
	std::string album;
	std::string year;
	std::string comment;
	std::uint8_t genre = 255;
};


#endif
