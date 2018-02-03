#pragma once

#include "Editor.h"

struct EditorState;
struct Renderer;
struct SDL_Rect;
struct TextEditor;
struct Label;
struct MessageBox;

#include "GenericSelector.h"

class FileSelector: public GenericSelector
{
	static const int filenameSize = 256;
	static const int filterSize = 256;

	enum
	{
		MessageBoxOverwrite
	};

	char mFilter[filterSize];
	char mFilename[filenameSize];
	Label *mFilenameLabel;
	TextEditor *mNameField;
	MessageBox *mMessageBox;
	bool mCheckOverwrite;

	struct FileItem: public Item {
		bool isDirectory;
		std::string path;
		int size;

		FileItem(bool isDirectory, const char *path, int size);

		// For sorting the listing
		static bool directorySort(const Item* a, const Item* b);
		static bool checkDirectory(const char *name);
		static const char *formatSize(int size);
	};

	virtual void onSelectItem(const Item& item);
	virtual void renderItem(Renderer& renderer, const SDL_Rect& area, const Item& item, bool isSelected);
	virtual void accept(bool isFinal = false);
	virtual void reject(bool isFinal = false);

	static bool fileExists(const char *path);

public:
	FileSelector(EditorState& editorState);
	virtual ~FileSelector();

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

	virtual bool onEvent(SDL_Event& event);
	virtual void onMessageBoxEvent(const Editor& messageBox, int code);
};
