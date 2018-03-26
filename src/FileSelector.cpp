#include "FileSelector.h"
#include "Renderer.h"
#include "Color.h"
#include "SDL.h"
#include "TextEditor.h"
#include "Label.h"
#include "MessageBox.h"
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>

FileSelector::FileSelector(EditorState& editorState)
	: GenericSelector(editorState), mCheckOverwrite(false)
{
	mNameField = new TextEditor(editorState);
	mNameField->setBuffer(mFilename, filenameSize);
	mNameField->setIsEditing(true);
	mNameField->setAlwaysShowCursor(true);
	mFilenameLabel = new Label(editorState);
	mFilenameLabel->setColor(Color(0, 0, 0));
	mFilenameLabel->setBackground(Color(255, 255, 255));
	mFilenameLabel->setText("FILENAME:");
	addChild(mFilenameLabel, 0, 8, 8*9, 8);
	addChild(mNameField, 8*9, 8, 208, 8);
	strcpy(mFilename, "");
	strcpy(mFilter, "");
	setFocus(mNameField);
	mMessageBox = new MessageBox(editorState);
}


FileSelector::~FileSelector()
{
	delete mNameField;
	delete mFilenameLabel;
	delete mMessageBox;
}


bool FileSelector::fileExists(const char *path)
{
	FILE *f = fopen(path, "r");

	if (!f)
		return false;

	fclose(f);

	return true;
}


void FileSelector::accept(bool isFinal)
{
	if (isFinal)
		mParent->onFileSelectorEvent(*this, true);
	else
	{
		if (FileItem::checkDirectory(getSelectedPath()))
		{
			setPath(getSelectedPath());
			return;
		}

		if (mCheckOverwrite && fileExists(getSelectedPath()))
		{
			mMessageBox->setTitle("Overwrite y/n");
			mMessageBox->setId(MessageBoxOverwrite);
			setModal(mMessageBox);
		}
		else
			mParent->onFileSelectorEvent(*this, true);
	}
}


void FileSelector::onMessageBoxEvent(const Editor& _messageBox, int code)
{
	const MessageBox& messageBox = reinterpret_cast<const MessageBox&>(_messageBox);

	if (messageBox.getId() == MessageBoxOverwrite)
	{
		if (code == 1)
		{
			accept(true);
		}
		else
		{
		}
	}

	setModal(NULL);
}


void FileSelector::reject(bool isFinal)
{
	mParent->onFileSelectorEvent(*this, false);
}


void FileSelector::renderItem(Renderer& renderer, const SDL_Rect& area, const Item& item, bool isSelected)
{
	const FileItem& fileItem = static_cast<const FileItem&>(item);
	Color color;

	if (isSelected)
		color = Color(255, 0, 0);

	renderer.clearRect(area, Color(0, 0, 0));

	int width = area.w / 8 - 10;

	if (fileItem.isDirectory)
		renderer.renderTextV(area, color, "<%*s>     DIR", -width, fileItem.path.c_str());
	else
		renderer.renderTextV(area, color, "%*s %9s", -width, fileItem.path.c_str(), FileItem::formatSize(fileItem.size));
}


void FileSelector::setPath(const char *path)
{
	chdir(path);
	setDirty(true);
	populate();
}


void FileSelector::populate()
{
	invalidateParent();

	clearItems();

	DIR *directory = opendir(".");

	if (!directory)
		return;

	while (true)
	{
		struct dirent *entry;
		struct stat statBuf;

		entry = readdir(directory);

		if (entry == NULL)
			break;

		if (!stat(entry->d_name, &statBuf) && ((statBuf.st_mode & S_IFDIR) ||
			(strlen(entry->d_name) > strlen(mFilter) && strcmp(entry->d_name + strlen(entry->d_name) - strlen(mFilter), mFilter) == 0)))
		{
			addItem(new FileItem(statBuf.st_mode & S_IFDIR, entry->d_name, statBuf.st_size));
		}
	}

	closedir(directory);

	sortItems(FileItem::directorySort);

	selectItem(0);
}


const char * FileSelector::getSelectedPath() const
{
	return mFilename;
}


void FileSelector::setFilter(const char *extension)
{
	strncpy(mFilter, extension, filterSize);
}


void FileSelector::setOverwriteCheck(bool state)
{
	mCheckOverwrite = state;
}


FileSelector::FileItem::FileItem(bool _isDirectory, const char *_path, int _size)
	: isDirectory(_isDirectory), path(_path), size(_size)
{
}


bool FileSelector::FileItem::directorySort(const FileSelector::Item* ga, const FileSelector::Item* gb)
{
	const FileItem& a = static_cast<const FileItem&>(*ga);
	const FileItem& b = static_cast<const FileItem&>(*gb);

	// Directories first

	if (a.isDirectory && !b.isDirectory)
		return true;

	// Then in alphabetical order if BOTH a & b are either directories or files

	if (a.isDirectory == b.isDirectory && strcmp(a.path.c_str(), b.path.c_str()) < 0)
		return true;

	return false;
}


bool FileSelector::FileItem::checkDirectory(const char *name)
{
	struct stat statBuf;

	if (!stat(name, &statBuf))
	{
		return (statBuf.st_mode & S_IFDIR) == S_IFDIR;
	}

	return false;
}


const char *FileSelector::FileItem::formatSize(int size)
{
	static char buffer[50];

	const struct { int divisor; const char *unit; } units[] = {
		{1, "B"},
		{1024, "k"},
		{1024 * 1024, "M"},
		{1024 * 1024 * 1024, "G"},
		{0, NULL}
	};

	int divisor = 0;
	const char *unit = NULL;

	for (int i = 0 ; units[i].divisor > 0 ; ++i)
	{
		if (size >= units[i].divisor)
		{
			divisor = units[i].divisor;
			unit = units[i].unit;
		}
	}

	snprintf(buffer, sizeof(buffer), "%d%s", size / divisor, unit);

	return buffer;
}


void FileSelector::onSelectItem(const Item& item)
{
	const FileItem& fileItem = static_cast<const FileItem&>(item);
	strncpy(mFilename, fileItem.path.c_str(), filenameSize);
	mNameField->setText(mFilename);
}


bool FileSelector::onEvent(SDL_Event& event)
{
	return GenericSelector::onEvent(event) || mNameField->onEvent(event);
}


void FileSelector::onModalStatusChange(bool isNowModal)
{
	// Make sure text field will receive SDL_TEXTINPUTs
	// and disables them after dialog close
	mNameField->setIsEditing(isNowModal);
}
