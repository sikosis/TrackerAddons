#include "EditorWindows.h"

#include "FileNameBuilder.h"
#include "Genres.h"

#include <algorithm>
#include <cstdlib>

#include <Alert.h>
#include <Button.h>
#include <CheckBox.h>
#include <Entry.h>
#include <LayoutBuilder.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <RadioButton.h>
#include <StringView.h>
#include <TextControl.h>
#include <Window.h>


namespace {


const uint32 kSave = 'save';
const uint32 kRename = 'renm';
const uint32 kRenameArtistTitle = 'rnar';
const uint32 kRenameTrackArtistTitle = 'rnta';
const uint32 kRenameArtistTrackTitle = 'rart';
const uint32 kRenameCustom = 'rncu';
const uint32 kCancel = 'cncl';
const uint32 kGenre = 'genr';


BMenuField* CreateGenreField(std::uint8_t selected, int32& selectedGenre)
{
	BPopUpMenu* menu = new BPopUpMenu("genre");
	for (std::uint16_t index : SortedGenreIndices()) {
		BMessage* message = new BMessage(kGenre);
		message->AddInt32("index", static_cast<int32>(index));
		BMenuItem* item = new BMenuItem(GenreName(index), message);
		menu->AddItem(item);
		if (index == selected)
			item->SetMarked(true);
	}
	if (selected >= kGenreCount)
		selectedGenre = 255;
	return new BMenuField("Genre:", menu);
}
//---------------------------------------------------------------------------------------------------------------------------------//


std::uint32_t ParseTrackNumber(const char* value)
{
	if (value == NULL || *value == '\0')
		return 0;
	char* end = NULL;
	const unsigned long track = std::strtoul(value, &end, 10);
	if (end == value || *end != '\0' || track > 999999)
		return 0;
	return static_cast<std::uint32_t>(track);
}
//---------------------------------------------------------------------------------------------------------------------------------//


class SingleEditorWindow : public BWindow {
public:
	SingleEditorWindow(const char* filename, BPath* path, TagData* data,
		bool* accepted)
		:
		BWindow(BRect(100, 100, 560, 390), "MP3 Tag", B_TITLED_WINDOW,
			B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS),
		fData(data),
		fAccepted(accepted),
		fPath(path),
		fSelectedGenre(data->genre)
	{
		fTitle = new BTextControl("Title:", data->title.c_str(), NULL);
		fArtist = new BTextControl("Artist:", data->artist.c_str(), NULL);
		fAlbum = new BTextControl("Album:", data->album.c_str(), NULL);
		fYear = new BTextControl("Year:", data->year.c_str(), NULL);
		fComment = new BTextControl("Comment:", data->comment.c_str(), NULL);
		fTrack = new BTextControl("Track:",
			data->track == 0 ? "" : std::to_string(data->track).c_str(), NULL);
		fRenamePattern = new BTextControl("Rename pattern:",
			kDefaultRenamePattern, NULL);
		fRenamePattern->SetToolTip(
			"Wildcards: {track}, {artist}, {title}, {album}, {year}");
		fRenamePattern->SetEnabled(false);
		BRadioButton* artistTitle = new BRadioButton("Artist - Title",
			new BMessage(kRenameArtistTitle));
		artistTitle->SetValue(B_CONTROL_ON);

		BButton* save = new BButton("Save", new BMessage(kSave));
		BButton* rename = new BButton("Rename File", new BMessage(kRename));
		fFilename = new BStringView("filename", filename);
		BLayoutBuilder::Group<>(this, B_VERTICAL)
			.SetInsets(B_USE_WINDOW_SPACING)
			.Add(fFilename)
			.AddGrid(B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING)
				.Add(fTitle->CreateLabelLayoutItem(), 0, 0)
				.Add(fTitle->CreateTextViewLayoutItem(), 1, 0)
				.Add(fArtist->CreateLabelLayoutItem(), 0, 1)
				.Add(fArtist->CreateTextViewLayoutItem(), 1, 1)
				.Add(fAlbum->CreateLabelLayoutItem(), 0, 2)
				.Add(fAlbum->CreateTextViewLayoutItem(), 1, 2)
				.Add(fComment->CreateLabelLayoutItem(), 0, 3)
				.Add(fComment->CreateTextViewLayoutItem(), 1, 3)
				.Add(fYear->CreateLabelLayoutItem(), 0, 4)
				.Add(fYear->CreateTextViewLayoutItem(), 1, 4)
				.Add(fTrack->CreateLabelLayoutItem(), 0, 5)
				.Add(fTrack->CreateTextViewLayoutItem(), 1, 5)
				.Add(CreateGenreField(data->genre, fSelectedGenre), 0, 6, 2)
			.End()
			.Add(new BStringView("renameFormat", "Rename format:"))
			.AddGrid(B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING)
				.Add(artistTitle, 0, 0)
				.Add(new BRadioButton("Track - Artist - Title",
					new BMessage(kRenameTrackArtistTitle)), 1, 0)
				.Add(new BRadioButton("Artist - Track - Title",
					new BMessage(kRenameArtistTrackTitle)), 0, 1)
				.Add(new BRadioButton("Custom",
					new BMessage(kRenameCustom)), 1, 1)
			.End()
			.Add(fRenamePattern)
			.AddGroup(B_HORIZONTAL)
				.Add(rename)
				.AddGlue()
				.Add(new BButton("Cancel", new BMessage(kCancel)))
				.Add(save)
			.End();
		SetDefaultButton(save);
	}

	void MessageReceived(BMessage* message) override
	{
		switch (message->what) {
			case kSave:
				_StoreFields();
				*fAccepted = true;
				Quit();
				break;
			case kRename:
			{
				_StoreFields();
				std::string filename;
				std::string error;
				const std::string pattern = fRenamePattern->Text();
				if (!BuildTagFileName(*fData, pattern, filename, error)) {
					(new BAlert("MP3 Tag", error.c_str(), "OK", NULL, NULL,
						B_WIDTH_AS_USUAL, B_WARNING_ALERT))->Go();
					break;
				}
				if (std::string(fPath->Leaf()) == filename) {
					fFilename->SetText(filename.c_str());
					break;
				}
				BEntry entry(fPath->Path(), true);
				if (entry.InitCheck() != B_OK) {
					(new BAlert("MP3 Tag", "Could not access the MP3 file for renaming.",
						"OK", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
					break;
				}
				const status_t status = entry.Rename(filename.c_str(), false);
				if (status != B_OK) {
					error = "Could not rename the file (error "
						+ std::to_string(status) + ").";
					(new BAlert("MP3 Tag", error.c_str(), "OK", NULL, NULL,
						B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
					break;
				}
				BPath renamedPath;
				if (entry.GetPath(&renamedPath) == B_OK)
					fPath->SetTo(renamedPath.Path());
				fFilename->SetText(filename.c_str());
				break;
			}
			case kRenameArtistTitle:
				_SelectRenamePattern(kDefaultRenamePattern, false);
				break;
			case kRenameTrackArtistTitle:
				_SelectRenamePattern(kTrackArtistTitlePattern, false);
				break;
			case kRenameArtistTrackTitle:
				_SelectRenamePattern(kArtistTrackTitlePattern, false);
				break;
			case kRenameCustom:
				_SelectRenamePattern(NULL, true);
				break;
			case kCancel:
				Quit();
				break;
			case kGenre:
				message->FindInt32("index", &fSelectedGenre);
				break;
			default:
				BWindow::MessageReceived(message);
		}
	}
//---------------------------------------------------------------------------------------------------------------------------------//


private:
	void _SelectRenamePattern(const char* pattern, bool custom)
	{
		if (pattern != NULL)
			fRenamePattern->SetText(pattern);
		fRenamePattern->SetEnabled(custom);
		if (custom)
			fRenamePattern->MakeFocus(true);
	}
//---------------------------------------------------------------------------------------------------------------------------------//


	void _StoreFields()
	{
		fData->title = fTitle->Text();
		fData->artist = fArtist->Text();
		fData->album = fAlbum->Text();
		fData->year = fYear->Text();
		fData->comment = fComment->Text();
		fData->track = ParseTrackNumber(fTrack->Text());
		fData->genre = static_cast<std::uint8_t>(fSelectedGenre);
	}
//---------------------------------------------------------------------------------------------------------------------------------//


	TagData* fData;
	bool* fAccepted;
	BPath* fPath;
	int32 fSelectedGenre;
	BTextControl* fTitle;
	BTextControl* fArtist;
	BTextControl* fAlbum;
	BTextControl* fYear;
	BTextControl* fComment;
	BTextControl* fTrack;
	BTextControl* fRenamePattern;
	BStringView* fFilename;
};


class BatchEditorWindow : public BWindow {
public:
	BatchEditorWindow(BatchOptions* options, bool* accepted)
		:
		BWindow(BRect(120, 120, 650, 560), "MP3 Tag — Multiple Files",
			B_TITLED_WINDOW, B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS),
		fOptions(options),
		fAccepted(accepted),
		fSelectedGenre(255)
	{
		fUsePattern = new BCheckBox("Extract fields from filename pattern", NULL);
		fPattern = new BTextControl("Pattern:", options->pattern.c_str(), NULL);
		fSetTitle = new BCheckBox("Title", NULL);
		fSetArtist = new BCheckBox("Artist", NULL);
		fSetAlbum = new BCheckBox("Album", NULL);
		fSetYear = new BCheckBox("Year", NULL);
		fSetComment = new BCheckBox("Comment", NULL);
		fSetTrack = new BCheckBox("Track", NULL);
		fSetGenre = new BCheckBox("Genre", NULL);
		fTitle = new BTextControl(NULL, "", NULL);
		fArtist = new BTextControl(NULL, "", NULL);
		fAlbum = new BTextControl(NULL, "", NULL);
		fYear = new BTextControl(NULL, "", NULL);
		fComment = new BTextControl(NULL, "", NULL);
		fTrack = new BTextControl(NULL, "", NULL);

		BButton* apply = new BButton("Apply", new BMessage(kSave));
		BLayoutBuilder::Group<>(this, B_VERTICAL)
			.SetInsets(B_USE_WINDOW_SPACING)
			.Add(new BStringView("help",
				"Pattern fields: /a artist, /t title, /n album, /y year, /c comment, /* wildcard"))
			.Add(fUsePattern)
			.Add(fPattern)
			.Add(new BStringView("fields", "Set the checked fields on every selected file:"))
			.AddGrid(B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING)
				.Add(fSetTitle, 0, 0).Add(fTitle, 1, 0)
				.Add(fSetArtist, 0, 1).Add(fArtist, 1, 1)
				.Add(fSetAlbum, 0, 2).Add(fAlbum, 1, 2)
				.Add(fSetComment, 0, 3).Add(fComment, 1, 3)
				.Add(fSetYear, 0, 4).Add(fYear, 1, 4)
				.Add(fSetTrack, 0, 5).Add(fTrack, 1, 5)
				.Add(fSetGenre, 0, 6)
				.Add(CreateGenreField(255, fSelectedGenre), 1, 6)
			.End()
			.AddGroup(B_HORIZONTAL)
				.AddGlue()
				.Add(new BButton("Cancel", new BMessage(kCancel)))
				.Add(apply)
			.End();
		SetDefaultButton(apply);
	}

	void MessageReceived(BMessage* message) override
	{
		switch (message->what) {
			case kSave:
				fOptions->usePattern = fUsePattern->Value() == B_CONTROL_ON;
				fOptions->pattern = fPattern->Text();
				fOptions->setTitle = fSetTitle->Value() == B_CONTROL_ON;
				fOptions->setArtist = fSetArtist->Value() == B_CONTROL_ON;
				fOptions->setAlbum = fSetAlbum->Value() == B_CONTROL_ON;
				fOptions->setYear = fSetYear->Value() == B_CONTROL_ON;
				fOptions->setComment = fSetComment->Value() == B_CONTROL_ON;
				fOptions->setTrack = fSetTrack->Value() == B_CONTROL_ON;
				fOptions->setGenre = fSetGenre->Value() == B_CONTROL_ON;
				fOptions->data.title = fTitle->Text();
				fOptions->data.artist = fArtist->Text();
				fOptions->data.album = fAlbum->Text();
				fOptions->data.year = fYear->Text();
				fOptions->data.comment = fComment->Text();
				fOptions->data.track = ParseTrackNumber(fTrack->Text());
				fOptions->data.genre = static_cast<std::uint8_t>(fSelectedGenre);
				*fAccepted = true;
				Quit();
				break;
			case kCancel:
				Quit();
				break;
			case kGenre:
				message->FindInt32("index", &fSelectedGenre);
				break;
			default:
				BWindow::MessageReceived(message);
		}
	}
//---------------------------------------------------------------------------------------------------------------------------------//


private:
	BatchOptions* fOptions;
	bool* fAccepted;
	int32 fSelectedGenre;
	BCheckBox* fUsePattern;
	BTextControl* fPattern;
	BCheckBox* fSetTitle;
	BCheckBox* fSetArtist;
	BCheckBox* fSetAlbum;
	BCheckBox* fSetYear;
	BCheckBox* fSetComment;
	BCheckBox* fSetGenre;
	BCheckBox* fSetTrack;
	BTextControl* fTitle;
	BTextControl* fArtist;
	BTextControl* fAlbum;
	BTextControl* fYear;
	BTextControl* fComment;
	BTextControl* fTrack;
};


} // namespace


bool EditSingleTag(BPath& path, TagData& data)
{
	bool accepted = false;
	SingleEditorWindow* window = new SingleEditorWindow(path.Leaf(), &path,
		&data, &accepted);
	window->Show();
	const thread_id thread = window->Thread();
	status_t result;
	wait_for_thread(thread, &result);
	return accepted;
}
//---------------------------------------------------------------------------------------------------------------------------------//


bool EditMultipleTags(BatchOptions& options)
{
	bool accepted = false;
	BatchEditorWindow* window = new BatchEditorWindow(&options, &accepted);
	window->Show();
	const thread_id thread = window->Thread();
	status_t result;
	wait_for_thread(thread, &result);
	return accepted;
}
//---------------------------------------------------------------------------------------------------------------------------------//
