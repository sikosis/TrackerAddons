#include "ClipboardText.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>


namespace {


void ExpectEqual(const std::string& actual, const std::string& expected,
	const char* testName)
{
	if (actual == expected)
		return;

	std::cerr << testName << ": expected [" << expected << "], got ["
		<< actual << "]\n";
	std::exit(1);
}
//---------------------------------------------------------------------------------------------------------------------------------//


} // namespace


int main()
{
	ExpectEqual(FormatClipboardText({}, false, false), "", "empty selection");
	ExpectEqual(FormatClipboardText({"/boot/home/file"}, false, false),
		"/boot/home/file", "single path");
	ExpectEqual(FormatClipboardText({"/boot/home/one", "/boot/home/two"},
		false, false), "/boot/home/one /boot/home/two", "space delimiter");
	ExpectEqual(FormatClipboardText({"/boot/home/one", "/boot/home/two"},
		false, true), "/boot/home/one\n/boot/home/two", "line delimiter");
	ExpectEqual(FormatClipboardText({"/boot/home/a file"}, false, false),
		"\"/boot/home/a file\"", "quoted path");
	ExpectEqual(FormatClipboardText({"/boot/home/a file"}, true, false),
		"\"a file\"", "name only");

	std::cout << "All ClipboardText tests passed.\n";
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------//
