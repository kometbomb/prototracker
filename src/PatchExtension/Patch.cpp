#include "Patch.h"
#include <cstdio>

PatchDescriptor::PatchDescriptor()
    : mNumPatchParams(0)
{
}


bool PatchDescriptor::registerParamInteger(int id, const char *name, int defaultValue, int minValue, int maxValue)
{
    if (mNumPatchParams >= maxPatchParams)
    {
        return false;
    }

    mParams[mNumPatchParams] = PatchParamDesc(id, PatchParamDesc::Integer, name, defaultValue, minValue, maxValue);

    mNumPatchParams++;

    return true;
}


const PatchParamDesc& PatchDescriptor::getParam(int paramIndex) const
{
    return mParams[paramIndex];
}


PatchParamDesc::PatchParamDesc()
{

}


PatchParamDesc::PatchParamDesc(int id, ParamType type, const char *name, int _defaultValue, int _minValue, int _maxValue)
    : paramId(id), paramType(type), paramName(name), defaultValue(_defaultValue), maxValue(_maxValue), minValue(_minValue)
{

}


int PatchDescriptor::getParamIndex(int paramId) const
{
    for (int i = 0 ; i < mNumPatchParams ; ++i)
    {
        if (mParams[i].paramId == paramId)
        {
            return i;
        }
    }

    return -1;
}


Patch::Patch()
{
}


int PatchDescriptor::getNumParams() const
{
    return mNumPatchParams;
}
