#pragma once

#include "SectionListener.h"

struct Patch;
struct PatchDescriptor;

class PatchManager: public SectionListener {
public:
    static const int maxPatches = 128;

private:
    Patch *mPatches;
    PatchDescriptor *mDescriptor;
    int getLastUsedPatch() const;
    bool readPatch(int index, const FileSection& section, int& offset);
    void writePatch(int index, FileSection& section);

public:
    PatchManager();
    ~PatchManager();

    PatchDescriptor& getDescriptor() const;
    Patch& getPatch(int index) const;

    /* Return false if failed
	 */
	virtual bool onFileSectionLoad(const FileSection& section, int& offset);
	virtual void onFileSectionSave(FileSection& section);

	// Triggered on Song::clear() if the Load event is listened to
	virtual void onSongClear();
};
