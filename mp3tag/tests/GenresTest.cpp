#include "Genres.h"

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>


namespace {


void Expect(bool condition, const char* name)
{
	if (condition)
		return;
	std::cerr << "FAILED: " << name << '\n';
	std::exit(1);
}
//---------------------------------------------------------------------------------------------------------------------------------//


std::string Lowercase(const char* value)
{
	std::string result = value;
	for (char& character : result) {
		character = static_cast<char>(std::tolower(
			static_cast<unsigned char>(character)));
	}
	return result;
}
//---------------------------------------------------------------------------------------------------------------------------------//


} // namespace


int main()
{
	const std::vector<std::uint16_t> indices = SortedGenreIndices();
	Expect(indices.size() == kGenreCount + 1, "all genres plus Unknown");
	for (std::size_t index = 1; index < indices.size(); ++index) {
		Expect(Lowercase(GenreName(indices[index - 1]))
			<= Lowercase(GenreName(indices[index])), "alphabetical order");
	}
	Expect(GenreName(255) == std::string("Unknown"), "Unknown mapping");

	std::cout << "All Genres tests passed.\n";
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------//
