#include "FileNameBuilder.h"

#include <cstdlib>
#include <iostream>
#include <string>


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
	TagData data;
	data.artist = "Harris Heller";
	data.title = "Heart Piece";
	std::string filename;
	std::string error;
	Expect(BuildTagFileName(data, kDefaultRenamePattern, filename, error),
		"valid tags");
	Expect(filename == "Harris Heller - Heart Piece.mp3", "expected filename");
	data.track = 4;
	Expect(BuildTagFileName(data, kTrackArtistTitlePattern,
		filename, error), "track number pattern");
	Expect(filename == "4 - Harris Heller - Heart Piece.mp3",
		"filename with track number");
	Expect(BuildTagFileName(data, kArtistTrackTitlePattern,
		filename, error), "artist track title pattern");
	Expect(filename == "Harris Heller - 4 - Heart Piece.mp3",
		"artist track title filename");

	data.artist = "  Artist/Name  ";
	data.title = " Track\\Name ";
	Expect(BuildTagFileName(data, kDefaultRenamePattern, filename, error),
		"sanitised tags");
	Expect(filename == "Artist Name - Track Name.mp3", "sanitised filename");

	data.artist.clear();
	Expect(!BuildTagFileName(data, kDefaultRenamePattern, filename, error),
		"missing artist");
	data.artist = "Artist";
	data.title.clear();
	Expect(!BuildTagFileName(data, kDefaultRenamePattern, filename, error),
		"missing title");
	data.title = "Title";
	data.track = 0;
	Expect(!BuildTagFileName(data, kTrackArtistTitlePattern,
		filename, error), "missing track number");
	Expect(!BuildTagFileName(data, "{unknown}", filename, error),
		"unknown wildcard");
	Expect(!BuildTagFileName(data, "{artist", filename, error),
		"unmatched wildcard");
	Expect(BuildTagFileName(data, "{artist} - {title}", filename, error),
		"automatic extension");
	Expect(filename == "Artist - Title.mp3", "extension appended");

	data.title = std::string(250, 'x');
	Expect(!BuildTagFileName(data, kDefaultRenamePattern, filename, error),
		"filename too long");

	std::cout << "All FileNameBuilder tests passed.\n";
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------//
