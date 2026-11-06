#include "Genres.h"

#include <algorithm>
#include <cctype>


const char* const kGenreNames[] = {
	"Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk",
	"Grunge", "Hip-Hop", "Jazz", "Metal", "New Age", "Oldies", "Other",
	"Pop", "R&B", "Rap", "Reggae", "Rock", "Techno", "Industrial",
	"Alternative", "Ska", "Death Metal", "Pranks", "Soundtrack",
	"Euro-Techno", "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk", "Fusion",
	"Trance", "Classical", "Instrumental", "Acid", "House", "Game",
	"Sound Clip", "Gospel", "Noise", "AlternRock", "Bass", "Soul", "Punk",
	"Space", "Meditative", "Instrumental Pop", "Instrumental Rock", "Ethnic",
	"Gothic", "Darkwave", "Techno-Industrial", "Electronic", "Pop-Folk",
	"Eurodance", "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta",
	"Top 40", "Christian Rap", "Pop/Funk", "Jungle", "Native American",
	"Cabaret", "New Wave", "Psychedelic", "Rave", "Showtunes", "Trailer",
	"Lo-Fi", "Tribal", "Acid Punk", "Acid Jazz", "Polka", "Retro",
	"Musical", "Rock & Roll", "Hard Rock", "Folk", "Folk/Rock",
	"National Folk", "Swing", "Fast Fusion", "Bebop", "Latin", "Revival",
	"Celtic", "Bluegrass", "Avantgarde", "Gothic Rock", "Progressive Rock",
	"Psychedelic Rock", "Symphonic Rock", "Slow Rock", "Big Band", "Chorus",
	"Easy Listening", "Acoustic", "Humour", "Speech", "Chanson", "Opera",
	"Chamber Music", "Sonata", "Symphony", "Booty Bass", "Primus",
	"Porn Groove", "Satire", "Slow Jam", "Club", "Tango", "Samba",
	"Folklore", "Ballad", "Power Ballad", "Rhythmic Soul", "Freestyle",
	"Duet", "Punk Rock", "Drum Solo", "A cappella", "Euro-House", "Dance Hall"
};


const std::size_t kGenreCount = sizeof(kGenreNames) / sizeof(kGenreNames[0]);


const char* GenreName(std::uint16_t index)
{
	return index < kGenreCount ? kGenreNames[index] : "Unknown";
}
//---------------------------------------------------------------------------------------------------------------------------------//


std::vector<std::uint16_t> SortedGenreIndices()
{
	std::vector<std::uint16_t> indices;
	indices.reserve(kGenreCount + 1);
	for (std::size_t index = 0; index < kGenreCount; ++index)
		indices.push_back(static_cast<std::uint16_t>(index));
	indices.push_back(255);

	std::sort(indices.begin(), indices.end(), [](std::uint16_t left,
		std::uint16_t right) {
		const unsigned char* leftName
			= reinterpret_cast<const unsigned char*>(GenreName(left));
		const unsigned char* rightName
			= reinterpret_cast<const unsigned char*>(GenreName(right));
		while (*leftName != '\0' && *rightName != '\0') {
			const int leftCharacter = std::tolower(*leftName);
			const int rightCharacter = std::tolower(*rightName);
			if (leftCharacter != rightCharacter)
				return leftCharacter < rightCharacter;
			++leftName;
			++rightName;
		}
		return *leftName < *rightName;
	});
	return indices;
}
//---------------------------------------------------------------------------------------------------------------------------------//
