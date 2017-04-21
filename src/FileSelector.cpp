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
	: Editor(editorState), mSelectedItem(0), mCheckOverwrite(false)
{
	mNameField = new TextEditor(editorState);
	mNameField->setBuffer(mFilename, filenameSize);
	mNameField->setIsEditing(true);
	mNameField->setAlwaysShowCursor(true);
	mLabel = new Label(editorState);
	mLabel->setColor(Color(0, 0, 0));
	mLabel->setBackground(Color(255, 255, 255));
	mFilenameLabel = new Label(editorState);
	mFilenameLabel->setColor(Color(0, 0, 0));
	mFilenameLabel->setBackground(Color(255, 255, 255));
	mFilenameLabel->setText("FILENAME:");
	addChild(mLabel, 0, 0, 280, 8);
	addChild(mFilenameLabel, 0, 8, 8*9, 8);
	addChild(mNameField, 8*9, 8, 208, 8);
	strcpy(mFilename, "");
	strcpy(mFilter, "");
	strcpy(mTitle, "");
	setFocus(mNameField);
	mMessageBox = new MessageBox(editorState);
}


FileSelector::~FileSelector()
{
	delete mNameField;
	delete mLabel;
	delete mFilenameLabel;
	delete mMessageBox;
}


void FileSelector::selectItem(int index)
{
	mSelectedItem = index;
	
	if (mSelectedItem >= static_cast<int>(mItems.size()))
		mSelectedItem = static_cast<int>(mItems.size()) - 1;
		
	if (mSelectedItem < 0)
		mSelectedItem = 0;
		
	if (mItems.size() > 0)
	{
		strncpy(mFilename, mItems[mSelectedItem].path.c_str(), filenameSize);
		mNameField->setText(mFilename);
	}
	
	setDirty(true);
}


void FileSelector::setTitle(const char *title)
{
	strncpy(mTitle, title, titleSize);
	mLabel->setText(mTitle);
	setModal(NULL);
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

	
bool FileSelector::onEvent(SDL_Event& event)
{
	if (mModal)
		return mModal->onEvent(event);
	
	switch (event.type)
	{
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				reject(true);
				return true;
			}
			
			if (event.key.keysym.sym == SDLK_RETURN)
			{
				accept(false);
				return true;
			}
			
			switch (event.key.keysym.sym)
			{
				case SDLK_UP:
					selectItem(mSelectedItem - 1);
					return true;
					
				case SDLK_DOWN:
					selectItem(mSelectedItem + 1);
					return true;
			}
			
			break;
	}
	
	return mNameField->onEvent(event);
}


void FileSelector::renderItem(Renderer& renderer, const SDL_Rect& area, const FileItem& item, bool isSelected)
{
	Color color;
	
	if (isSelected)
		color = Color(255, 0, 0);
	
	renderer.clearRect(area, Color(0, 0, 0));
	
	if (item.isDirectory)
		renderer.renderTextV(area, color, "<%-25s>     DIR", item.path.c_str());
	else
		renderer.renderTextV(area, color, "%-25s %9d", item.path.c_str(), item.size);
}

	
void FileSelector::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	if (shouldRedrawBackground())
	{
		renderer.clearRect(area, Color(0, 0, 0));
		
		/*SDL_Rect fieldArea = { area.x, area.y + 8, area.w, renderer.getFontHeight() };
		
		renderer.clearRect(fieldArea, Color(255, 0, 0));*/
	}
	
	int countVisible = (area.h - 8 - 8 - 8) / 8;
	int firstVisible = mSelectedItem - countVisible / 2;
	
	if (firstVisible < 0)
	{
		firstVisible = 0;
	}	
	
	int lastVisible = firstVisible + countVisible;
	
	if (lastVisible >= mItems.size())
	{
		lastVisible = mItems.size() - 1;
		firstVisible = lastVisible - countVisible;
		if (firstVisible < 0)
		{
			firstVisible = 0;
		}	
	}
	
	for (int row = firstVisible ; row <= lastVisible ; ++row)
	{
		SDL_Rect textArea = {area.x, (row - firstVisible) * 8 + area.y + 16, area.w, 8};
		renderItem(renderer, textArea, mItems[row], row == mSelectedItem);
	}
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
	
	mSelectedItem = 0;
	mItems.clear();
	
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
		
		//if (strlen(entry->d_name) > strlen(mFilter) && strcmp(entry->d_name + strlen(entry->d_name) - strlen(mFilter), mFilter) == 0)
		
		if (!stat(entry->d_name, &statBuf))
		{
			mItems.push_back(FileItem(statBuf.st_mode & S_IFDIR, entry->d_name, statBuf.st_size));
		}
	}
	
	closedir(directory);
	
	std::sort(mItems.begin(), mItems.end(), FileItem::directorySort);
	
	selectItem(0);
}


const char * FileSelector::getSelectedPath() const
{
	return mFilename;
}


const FileSelector::FileItem& FileSelector::getSelectedItem() const
{
	return mItems[mSelectedItem];
}



int FileSelector::getId() const
{
	return mId;
}


void FileSelector::setId(int id)
{
	mId = id;
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


bool FileSelector::FileItem::directorySort(const FileSelector::FileItem& a, const FileSelector::FileItem& b)
{
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

