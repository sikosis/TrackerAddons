#include "ClipboardText.h"

#include <cstddef>


namespace {


std::string LeafName(const std::string& path)
{
	const std::string::size_type separator = path.find_last_of('/');
	if (separator == std::string::npos)
		return path;
	return path.substr(separator + 1);
}
//---------------------------------------------------------------------------------------------------------------------------------//


std::string QuoteIfNeeded(const std::string& value)
{
	if (value.find(' ') == std::string::npos)
		return value;
	return '"' + value + '"';
}
//---------------------------------------------------------------------------------------------------------------------------------//


} // namespace


std::string FormatClipboardText(const std::vector<std::string>& paths,
	bool namesOnly, bool onePerLine)
{
	std::string result;
	const char* delimiter = onePerLine ? "\n" : " ";

	for (std::size_t index = 0; index < paths.size(); ++index) {
		if (index != 0)
			result += delimiter;

		const std::string value = namesOnly ? LeafName(paths[index]) : paths[index];
		result += QuoteIfNeeded(value);
	}

	return result;
}
//---------------------------------------------------------------------------------------------------------------------------------//
