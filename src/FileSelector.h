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
		static bool checkDirectory(const char *name);
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
	
	/* Set dialog id (freely set and used by the Editor that creates the dialog)
	 */
	void setId(int id);
	int getId() const;

	/* Set dialog title
	 */
	void setTitle(const char *title);

	/* Set path & populate list
	 */
	void setPath(const char *path);

	/* Set file extension list (with leading period)
	 */
	void setFilter(const char *extension);

	/* Read file list from the path set with setPath()
	 * using the extension filter set by setFilter()
	 */
	void populate();

	/* Whether the dialog should check if a file exists
	 * with the selected filename (for save dialogs)
	 */
	void setOverwriteCheck(bool state);

	/* After the dialog finishes this will return the path
	 * and filename to selected (or new) file
	 */
	const char * getSelectedPath() const;
	const FileItem& getSelectedItem() const;

	virtual bool onEvent(SDL_Event& event);
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
	virtual void onMessageBoxEvent(const Editor& messageBox, int code);
};
