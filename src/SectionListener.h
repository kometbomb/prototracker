#pragma once

struct FileSection;

/* SectionListener listens to unhandled Song FileSections
 * e.g. ISynth can then access the file to load/save its own settings
 */

class SectionListener
{
public:
	enum Flags {
		Load = 1,
		Save = 2
	};

	/* Return false if failed
	 */
	virtual bool onFileSectionLoad(const FileSection& section, int& offset);
	virtual void onFileSectionSave(FileSection& section);

	// Triggered on Song::clear() if the Load event is listened to
	virtual void onSongClear();
};
