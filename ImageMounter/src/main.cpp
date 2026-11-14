/*
 * Copyright 2002 Maurice Michalski
 * Copyright 2026 ImageMounter contributors
 */

#include "MountPoint.h"

#include <cerrno>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <vector>

#include <Alert.h>
#include <Application.h>
#include <Entry.h>
#include <LayoutBuilder.h>
#include <Message.h>
#include <Path.h>
#include <StringView.h>
#include <View.h>
#include <Window.h>
#include <add-ons/tracker/TrackerAddOn.h>
#include <fs_info.h>
#include <unistd.h>


namespace {


const char* const kSignature = "application/x-vnd.ImageMounter";
const uint32 kReadOnlyMount = 2;


struct MountResult {
	std::string image;
	std::string mountPoint;
	std::string filesystem;
	status_t status = B_ERROR;
};


MountResult MountImage(const char* imagePath)
{
	static const char* const filesystems[] = {
		"bfs", "hfs", "iso9660", "fat", "dos"
	};
	MountResult result;
	result.image = imagePath;
	const std::string name = MountNameForImage(imagePath);

	for (const char* filesystem : filesystems) {
		for (unsigned int number = 0; number < 1000; ++number) {
			const std::string mountPoint = NumberedMountPoint(name, number);
			BEntry entry(mountPoint.c_str());
			if (entry.Exists())
				continue;
			if (mkdir(mountPoint.c_str(), 0755) != 0) {
				result.status = errno;
				return result;
			}

			const dev_t device = mount(filesystem, mountPoint.c_str(), imagePath,
				kReadOnlyMount, NULL, 0);
			if (device >= B_OK) {
				result.mountPoint = mountPoint;
				result.filesystem = filesystem;
				result.status = B_OK;
				return result;
			}
			const status_t mountError = device == -1 ? errno : device;
			rmdir(mountPoint.c_str());
			result.status = mountError;
			break;
		}
	}
	return result;
}
//---------------------------------------------------------------------------------------------------------------------------------//


std::vector<MountResult> MountReferences(BMessage* message)
{
	std::vector<MountResult> results;
	if (message == NULL)
		return results;

	entry_ref reference;
	for (int32 index = 0;
		message->FindRef("refs", index, &reference) == B_OK; ++index) {
		BEntry entry(&reference, true);
		BPath path;
		if (entry.InitCheck() != B_OK || entry.GetPath(&path) != B_OK) {
			MountResult result;
			result.image = reference.name == NULL ? "Unknown image" : reference.name;
			result.status = B_ENTRY_NOT_FOUND;
			results.push_back(result);
			continue;
		}
		results.push_back(MountImage(path.Path()));
	}
	return results;
}
//---------------------------------------------------------------------------------------------------------------------------------//


std::string ResultMessage(const std::vector<MountResult>& results)
{
	if (results.empty())
		return "No image files were supplied.";

	std::string message;
	for (const MountResult& result : results) {
		const std::string::size_type separator = result.image.find_last_of('/');
		const std::string leaf = separator == std::string::npos
			? result.image : result.image.substr(separator + 1);
		if (result.status == B_OK) {
			message += leaf + " mounted at " + result.mountPoint + " ("
				+ result.filesystem + ").\n";
		} else {
			message += leaf + " could not be mounted: "
				+ std::strerror(result.status) + ".\n";
		}
	}
	return message;
}
//---------------------------------------------------------------------------------------------------------------------------------//


bool AllMounted(const std::vector<MountResult>& results)
{
	if (results.empty())
		return false;
	for (const MountResult& result : results) {
		if (result.status != B_OK)
			return false;
	}
	return true;
}
//---------------------------------------------------------------------------------------------------------------------------------//


void ShowResults(const std::vector<MountResult>& results)
{
	const std::string message = ResultMessage(results);
	(new BAlert("ImageMounter", message.c_str(), "OK", NULL, NULL,
		B_WIDTH_AS_USUAL, AllMounted(results) ? B_INFO_ALERT : B_WARNING_ALERT))->Go();
}
//---------------------------------------------------------------------------------------------------------------------------------//


class DropWindow : public BWindow {
public:
	DropWindow()
		:
		BWindow(BRect(100, 100, 420, 190), "ImageMounter", B_TITLED_WINDOW,
			B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS)
	{
		BStringView* heading = new BStringView("heading", "Mount a filesystem image");
		heading->SetFont(be_bold_font);
		BStringView* instructions = new BStringView("instructions",
			"Drop one or more image files here.");
		BLayoutBuilder::Group<>(this, B_VERTICAL)
			.SetInsets(B_USE_WINDOW_SPACING)
			.AddGlue()
			.Add(heading)
			.Add(instructions)
			.AddGlue();
	}

	bool QuitRequested() override
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return false;
	}
	//---------------------------------------------------------------------------------------------------------------------------------//


	void MessageReceived(BMessage* message) override
	{
		if (message->what == B_SIMPLE_DATA || message->HasRef("refs")) {
			be_app->PostMessage(message);
			return;
		}
		BWindow::MessageReceived(message);
	}
	//---------------------------------------------------------------------------------------------------------------------------------//
};


class ImageMounterApplication : public BApplication {
public:
	ImageMounterApplication()
		:
		BApplication(kSignature),
		fReceivedReferences(false)
	{
	}
	//---------------------------------------------------------------------------------------------------------------------------------//


	void ReadyToRun() override
	{
		if (fReceivedReferences) {
			PostMessage(B_QUIT_REQUESTED);
			return;
		}
		(new DropWindow())->Show();
	}
	//---------------------------------------------------------------------------------------------------------------------------------//


	void RefsReceived(BMessage* message) override
	{
		fReceivedReferences = true;
		ShowResults(MountReferences(message));
		if (CountWindows() == 0)
			PostMessage(B_QUIT_REQUESTED);
	}
	//---------------------------------------------------------------------------------------------------------------------------------//


	void MessageReceived(BMessage* message) override
	{
		if (message->what == B_SIMPLE_DATA || message->HasRef("refs")) {
			RefsReceived(message);
			return;
		}
		BApplication::MessageReceived(message);
	}
	//---------------------------------------------------------------------------------------------------------------------------------//


private:
	bool fReceivedReferences;
};


} // namespace


extern "C" void process_refs(entry_ref directoryReference, BMessage* message,
	void* reserved)
{
	(void)directoryReference;
	(void)reserved;
	ShowResults(MountReferences(message));
}
//---------------------------------------------------------------------------------------------------------------------------------//


int main()
{
	ImageMounterApplication application;
	application.Run();
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------//
