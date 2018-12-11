#include "PatchManager.h"
#include "Patch.h"
#include "FileSection.h"
#include <cstring>
#include <algorithm>

PatchManager::PatchManager()
{
    mPatches = new Patch[maxPatches];
    mDescriptor = new PatchDescriptor();

    mDescriptor->registerParam(0, PatchParamDesc::Integer, "Test", 0);
}


PatchManager::~PatchManager()
{
    delete [] mPatches;
    delete mDescriptor;
}


bool PatchManager::onFileSectionLoad(const FileSection& section, int& offset)
{
    if (strcmp(section.getName(), "PTCH") == 0)
    {
        int numPatches = section.readByte(offset);
        if (numPatches == FileSection::invalidRead)
        {
            return false;
        }

        if (numPatches > maxPatches)
        {
            return false;
        }

        for (int i = 0 ; i < numPatches ; ++i) {
            readPatch(i, section, offset);
        }
    }

    return true;
}


void PatchManager::onFileSectionSave(FileSection& section)
{
    if (strcmp(section.getName(), "PTCH") == 0)
    {
        const int numUsedPatches = getLastUsedPatch() + 1;
        section.writeByte(numUsedPatches);

        for (int i = 0 ; i < numUsedPatches ; ++i) {
            writePatch(i, section);
        }
    }
}


int PatchManager::getLastUsedPatch() const
{
    const PatchDescriptor& descriptor = getDescriptor();

    for (int i = 0 ; i < maxPatches ; ++i)
    {
        const Patch& patch = getPatch(i);
        for (int param = 0 ; param < descriptor.getNumParams() ; ++param)
        {
            if (patch.paramValue[i] != descriptor.getParam(param).defaultValue)
            {
                return i;
            }
        }
    }

    return -1;
}


void PatchManager::onSongClear()
{
    for (int i = 0 ; i < maxPatches ; ++i)
    {
        strcpy(mPatches[i].patchName, "");
        for (int paramIndex = 0 ; paramIndex < PatchDescriptor::maxPatchParams ; ++paramIndex)
        {
            mPatches[i].paramValue[paramIndex] = getDescriptor().getParam(paramIndex).defaultValue;
        }
    }
}


Patch& PatchManager::getPatch(int index) const
{
    return mPatches[std::min(maxPatches - 1, std::max(0, index))];
}


bool PatchManager::readPatch(int index, const FileSection& section, int& offset)
{
    Patch& patch = getPatch(index);
    const char *name = section.readString(offset);

    if (name == NULL) {
        return false;
    }

    strncpy(patch.patchName, name, Patch::patchNameLength - 1);

    for (int i = 0 ; i < PatchDescriptor::maxPatchParams ; ++i)
    {
        int id = section.readDword(offset);
        if (id == FileSection::invalidRead) {
            return false;
        }

        int value = section.readDword(offset);
        if (value == FileSection::invalidRead) {
            return false;
        }

        int paramIndex = getDescriptor().getParamIndex(id);

        if (paramIndex < 0)
        {
            return false;
        }

        patch.paramValue[paramIndex] = value;
    }

    return true;
}


void PatchManager::writePatch(int index, FileSection& section)
{
    int numParams = getDescriptor().getNumParams();
    const Patch& patch = getPatch(index);
    section.writeString(patch.patchName);
    section.writeByte(numParams);

    for (int i = 0 ; i < numParams ; ++i)
    {
        section.writeDword(getDescriptor().getParam(i).paramId);
        section.writeDword(patch.paramValue[i]);
    }
}


PatchDescriptor& PatchManager::getDescriptor() const
{
    return *mDescriptor;
}
