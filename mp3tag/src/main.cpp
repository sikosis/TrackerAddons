/*
 * Copyright 1999 Arvid Norberg
 * Copyright 2026 mp3tag maintainers
 */

#include "EditorWindows.h"
#include "Id3v1Tag.h"
#include "PatternMatcher.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <string>
#include <vector>

#include <Alert.h>
#include <Entry.h>
#include <Message.h>
#include <Node.h>
#include <NodeInfo.h>
#include <Path.h>
#include <add-ons/tracker/TrackerAddOn.h>


namespace {


bool IsMp3File(const BPath& path)
{
	std::string name = path.Leaf();
	std::transform(name.begin(), name.end(), name.begin(),
		[](unsigned char character) { return std::tolower(character); });
	return name.size() >= 4 && name.compare(name.size() - 4, 4, ".mp3") == 0;
}
//---------------------------------------------------------------------------------------------------------------------------------//


void ShowError(const std::string& message)
{
	(new BAlert("MP3 Tag", message.c_str(), "OK", NULL, NULL,
		B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
}
//---------------------------------------------------------------------------------------------------------------------------------//


void SetMp3MimeType(const entry_ref& reference)
{
	BNode node(&reference);
	if (node.InitCheck() != B_OK)
		return;
	BNodeInfo info(&node);
	info.SetType("audio/mpeg");
}
//---------------------------------------------------------------------------------------------------------------------------------//


bool LoadReference(const entry_ref& reference, BPath& path, TagData& data,
	std::string& error)
{
	BEntry entry(&reference, true);
	if (entry.InitCheck() != B_OK || entry.GetPath(&path) != B_OK) {
		error = "Could not resolve the selected file.";
		return false;
	}
	if (!IsMp3File(path)) {
		error = std::string(path.Leaf()) + " is not an MP3 file.";
		return false;
	}
	Id3v1Tag tag(path.Path());
	return tag.Load(data, error);
}
//---------------------------------------------------------------------------------------------------------------------------------//


bool SaveReference(const entry_ref& reference, const BPath& path,
	const TagData& data, std::string& error)
{
	Id3v1Tag tag(path.Path());
	TagData ignored;
	if (!tag.Load(ignored, error) || !tag.Save(data, error))
		return false;
	SetMp3MimeType(reference);
	return true;
}
//---------------------------------------------------------------------------------------------------------------------------------//


void ApplyPattern(const PatternResult& pattern, TagData& data)
{
	if (pattern.hasTitle)
		data.title = pattern.data.title;
	if (pattern.hasArtist)
		data.artist = pattern.data.artist;
	if (pattern.hasAlbum)
		data.album = pattern.data.album;
	if (pattern.hasYear)
		data.year = pattern.data.year;
	if (pattern.hasComment)
		data.comment = pattern.data.comment;
}
//---------------------------------------------------------------------------------------------------------------------------------//


void ApplyBatchFields(const BatchOptions& options, TagData& data)
{
	if (options.setTitle)
		data.title = options.data.title;
	if (options.setArtist)
		data.artist = options.data.artist;
	if (options.setAlbum)
		data.album = options.data.album;
	if (options.setYear)
		data.year = options.data.year;
	if (options.setComment)
		data.comment = options.data.comment;
	if (options.setGenre)
		data.genre = options.data.genre;
}
//---------------------------------------------------------------------------------------------------------------------------------//


} // namespace


extern "C" void process_refs(entry_ref directoryReference, BMessage* message,
	void* reserved)
{
	(void)directoryReference;
	(void)reserved;
	if (message == NULL)
		return;

	std::vector<entry_ref> references;
	entry_ref reference;
	for (int32 index = 0;
		message->FindRef("refs", index, &reference) == B_OK; ++index) {
		references.push_back(reference);
	}
	if (references.empty()) {
		ShowError("Select one or more MP3 files first.");
		return;
	}

	if (references.size() == 1) {
		BPath path;
		TagData data;
		std::string error;
		if (!LoadReference(references[0], path, data, error)) {
			ShowError(error);
			return;
		}
		if (!EditSingleTag(path.Leaf(), data))
			return;
		if (!SaveReference(references[0], path, data, error))
			ShowError(error);
		return;
	}

	BatchOptions options;
	if (!EditMultipleTags(options))
		return;

	std::vector<std::string> errors;
	for (const entry_ref& item : references) {
		BPath path;
		TagData data;
		std::string error;
		if (!LoadReference(item, path, data, error)) {
			errors.push_back(error);
			continue;
		}

		if (options.usePattern) {
			PatternResult pattern;
			if (!MatchFilenamePattern(path.Leaf(), options.pattern, pattern)) {
				errors.push_back(std::string(path.Leaf())
					+ " did not match the filename pattern.");
				continue;
			}
			ApplyPattern(pattern, data);
		}
		ApplyBatchFields(options, data);
		if (!SaveReference(item, path, data, error))
			errors.push_back(std::string(path.Leaf()) + ": " + error);
	}

	if (!errors.empty()) {
		std::string summary = "Some files could not be updated:\n\n";
		const std::size_t count = std::min<std::size_t>(errors.size(), 10);
		for (std::size_t index = 0; index < count; ++index)
			summary += "• " + errors[index] + "\n";
		if (errors.size() > count)
			summary += "• Additional errors were omitted.\n";
		ShowError(summary);
	}
}
//---------------------------------------------------------------------------------------------------------------------------------//


int main()
{
	std::fprintf(stderr, "This program can only be used as a Tracker add-on.\n");
	return 1;
}
//---------------------------------------------------------------------------------------------------------------------------------//
