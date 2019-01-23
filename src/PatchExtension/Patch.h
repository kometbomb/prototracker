#pragma once

struct PatchParamDesc
{
    enum ParamType {
        Integer,
    };

    int paramId;
    int defaultValue;
    const char *paramName;
    ParamType paramType;

    PatchParamDesc();
    PatchParamDesc(int id, ParamType type, const char *name, int defaultValue);
};

class PatchDescriptor
{
public:
    static const int maxPatchParams = 64;
    bool registerParam(int id, PatchParamDesc::ParamType type, const char *name, int defaultValue);
    int getNumParams() const;
    const PatchParamDesc& getParam(int paramIndex) const;
    int getParamIndex(int paramId) const;

    PatchDescriptor();

private:
    int mNumPatchParams;
    PatchParamDesc mParams[maxPatchParams];
};

struct Patch {
    static const int patchNameLength = 32;
    char patchName[patchNameLength];
    int paramValue[PatchDescriptor::maxPatchParams];

    Patch();
};
