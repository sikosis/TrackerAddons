#include "PatternMatcher.h"

#include <cstdlib>
#include <iostream>


namespace {


void Expect(bool condition, const char* name)
{
	if (condition)
		return;
	std::cerr << "FAILED: " << name << '\n';
	std::exit(1);
}
//---------------------------------------------------------------------------------------------------------------------------------//


} // namespace


int main()
{
	PatternResult result;
	Expect(MatchFilenamePattern("Artist - Title.mp3", "/a - /t.mp3", result),
		"artist and title match");
	Expect(result.hasArtist && result.data.artist == "Artist", "artist capture");
	Expect(result.hasTitle && result.data.title == "Title", "title capture");

	Expect(MatchFilenamePattern("1999 - Album - Song.mp3",
		"/y - /n - /t.mp3", result), "year album title match");
	Expect(result.data.year == "1999", "year capture");
	Expect(result.data.album == "Album", "album capture");
	Expect(result.data.title == "Song", "second title capture");

	Expect(MatchFilenamePattern("01 Artist - Song.mp3", "/* /a - /t.mp3", result),
		"wildcard match");
	Expect(result.data.artist == "Artist", "capture after wildcard");
	Expect(!MatchFilenamePattern("Artist_Title.mp3", "/a - /t.mp3", result),
		"literal mismatch");
	Expect(!MatchFilenamePattern("anything", "/q", result), "invalid token");
	Expect(MatchFilenamePattern(std::string(40, 'a') + ".mp3", "/a.mp3", result),
		"long capture match");
	Expect(result.data.artist.size() == 30, "capture clipped to ID3v1 width");

	std::cout << "All PatternMatcher tests passed.\n";
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------//
