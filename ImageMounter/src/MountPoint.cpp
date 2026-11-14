#include "MountPoint.h"

#include <algorithm>
#include <cctype>


namespace {


bool IsImageExtension(const std::string& extension)
{
	static const char* const extensions[] = {
		".bin", ".bfs", ".dmg", ".hfs", ".img", ".image", ".iso", ".raw"
	};
	return std::find(std::begin(extensions), std::end(extensions), extension)
		!= std::end(extensions);
}
//---------------------------------------------------------------------------------------------------------------------------------//


std::string Lowercase(const std::string& value)
{
	std::string result = value;
	std::transform(result.begin(), result.end(), result.begin(),
		[](unsigned char character) { return std::tolower(character); });
	return result;
}
//---------------------------------------------------------------------------------------------------------------------------------//


} // namespace


std::string MountNameForImage(const std::string& path)
{
	const std::string::size_type separator = path.find_last_of("/\\");
	std::string name = separator == std::string::npos
		? path : path.substr(separator + 1);
	const std::string::size_type dot = name.find_last_of('.');
	if (dot != std::string::npos
		&& IsImageExtension(Lowercase(name.substr(dot)))) {
		name.erase(dot);
	}

	for (char& character : name) {
		const unsigned char value = static_cast<unsigned char>(character);
		if (character == '/' || character == '\\' || std::iscntrl(value))
			character = '_';
	}
	while (!name.empty() && std::isspace(static_cast<unsigned char>(name.front())))
		name.erase(name.begin());
	while (!name.empty() && std::isspace(static_cast<unsigned char>(name.back())))
		name.pop_back();
	if (name.empty() || name == "." || name == "..")
		name = "ImageMounter";
	if (name.size() > 120)
		name.resize(120);
	return name;
}
//---------------------------------------------------------------------------------------------------------------------------------//


std::string NumberedMountPoint(const std::string& name, unsigned int number)
{
	return std::string("/") + name
		+ (number == 0 ? std::string() : " " + std::to_string(number));
}
//---------------------------------------------------------------------------------------------------------------------------------//
