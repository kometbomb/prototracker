#pragma once

#include "Editor.h"

struct EditorState;
struct Renderer;
struct SDL_Rect;
struct TextEditor;
struct Label;
struct MessageBox;

#include <vector>
#include <string>

class FileSelector: public Editor
{
	static const int filenameSize = 256;
	static const int titleSize = 256;
	static const int filterSize = 256;
	
	enum 
	{
		MessageBoxOverwrite
	};
	
	int mId;
	char mTitle[titleSize];
	char mFilter[filterSize];
	char mFilename[filenameSize];
	TextEditor *mNameField;
	Label *mLabel, *mFilenameLabel;
	MessageBox *mMessageBox;
	int mSelectedItem;
	bool mCheckOverwrite;
	
	struct FileItem {
		bool isDirectory;
		std::string path;
		int size;
		
		FileItem(bool isDirectory, const char *path, int size);
		
		// For sorting the listing
		static bool directorySort(const FileItem& a, const FileItem& b);
	};
	
	std::vector<FileItem> mItems;
	
	void selectItem(int index);
	void renderItem(Renderer& renderer, const SDL_Rect& area, const FileItem& item, bool isSelected);
	void accept(bool isFinal = false);
	void reject(bool isFinal = false);
	
	static bool fileExists(const char *path);
	
public:
	FileSelector(EditorState& editorState);
	virtual ~FileSelector();
	
	void populate();
	void setId(int id);
	void setTitle(const char *title);
	void setPath(const char *path);
	void setFilter(const char *extension);
	void setOverwriteCheck(bool state);
	const char * getSelectedPath() const;
	const FileItem& getSelectedItem() const;
	int getId() const;
	
	virtual bool onEvent(SDL_Event& event);
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
	virtual void onMessageBoxEvent(const Editor& messageBox, int code);
};
