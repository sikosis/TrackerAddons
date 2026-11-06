#include "Id3v1Tag.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
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


std::streamoff FileSize(const std::string& path)
{
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	return file.tellg();
}
//---------------------------------------------------------------------------------------------------------------------------------//


} // namespace


int main(int argc, char** argv)
{
	Expect(argc == 2, "temporary path argument");
	const std::string path = argv[1];
	TagData missingData;
	std::string missingError;
	Id3v1Tag missing(path + ".missing");
	Expect(!missing.Load(missingData, missingError), "missing file fails cleanly");
	{
		std::ofstream file(path, std::ios::binary);
		file << "audio payload";
	}

	TagData data;
	std::string error;
	Id3v1Tag tag(path);
	Expect(tag.Load(data, error), "load untagged file");
	data.title = "A title";
	data.artist = "An artist";
	data.album = "An album";
	data.year = "2026";
	data.comment = "A comment";
	data.genre = 17;
	Expect(tag.Save(data, error), "append tag");
	const std::streamoff taggedSize = FileSize(path);
	Expect(taggedSize == 13 + 128, "tag adds 128 bytes");

	TagData loaded;
	Id3v1Tag reloaded(path);
	Expect(reloaded.Load(loaded, error), "reload tag");
	Expect(loaded.title == data.title, "title round trip");
	Expect(loaded.artist == data.artist, "artist round trip");
	Expect(loaded.album == data.album, "album round trip");
	Expect(loaded.year == data.year, "year round trip");
	Expect(loaded.comment == data.comment, "comment round trip");
	Expect(loaded.genre == data.genre, "genre round trip");

	loaded.title = std::string(40, 'x');
	Expect(reloaded.Save(loaded, error), "replace tag");
	Expect(FileSize(path) == taggedSize, "replacement does not grow file");
	TagData clipped;
	Id3v1Tag clippedTag(path);
	Expect(clippedTag.Load(clipped, error), "reload clipped tag");
	Expect(clipped.title.size() == 30, "title clipped to 30 bytes");

	std::remove(path.c_str());
	std::cout << "All Id3v1Tag tests passed.\n";
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------//
