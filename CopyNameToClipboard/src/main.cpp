/*
 * Copyright 2000-2012 Werner Freytag and Humdinger
 * Copyright 2026 CopyNameToClipboard contributors
 * Distributed under the terms of the MIT licence.
 */

#include "ClipboardText.h"

#include <cstdio>
#include <vector>

#include <Clipboard.h>
#include <Entry.h>
#include <InterfaceDefs.h>
#include <Message.h>
#include <Mime.h>
#include <Path.h>
#include <SupportDefs.h>
#include <add-ons/tracker/TrackerAddOn.h>


extern "C" void process_refs(entry_ref directoryReference, BMessage* message,
	void* reserved)
{
	(void)reserved;

	if (message == NULL)
		return;

	std::vector<std::string> paths;
	entry_ref reference;
	for (int32 index = 0;
		message->FindRef("refs", index, &reference) == B_OK; ++index) {
		BEntry entry(&reference, true);
		BPath path;
		if (entry.InitCheck() == B_OK && entry.GetPath(&path) == B_OK)
			paths.push_back(path.Path());
	}

	if (paths.empty()) {
		BEntry directory(&directoryReference, true);
		BPath path;
		if (directory.InitCheck() != B_OK || directory.GetPath(&path) != B_OK)
			return;
		paths.push_back(path.Path());
	}

	const uint32 keys = modifiers();
	const std::string text = FormatClipboardText(paths,
		(keys & B_SHIFT_KEY) != 0, (keys & B_CONTROL_KEY) != 0);

	if (!be_clipboard->Lock())
		return;

	be_clipboard->Clear();
	BMessage* data = be_clipboard->Data();
	if (data != NULL
		&& data->AddData("text/plain", B_MIME_TYPE, text.c_str(),
			text.size()) == B_OK) {
		be_clipboard->Commit();
	}
	be_clipboard->Unlock();
}
//---------------------------------------------------------------------------------------------------------------------------------//


int main()
{
	std::fprintf(stderr, "This program can only be used as a Tracker add-on.\n");
	return 1;
}
//---------------------------------------------------------------------------------------------------------------------------------//
