#ifndef CLIPBOARD_TEXT_H
#define CLIPBOARD_TEXT_H

#include <string>
#include <vector>

std::string FormatClipboardText(const std::vector<std::string>& paths,
	bool namesOnly, bool onePerLine);

#endif
