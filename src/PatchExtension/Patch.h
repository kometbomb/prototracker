#pragma once

struct PatchParamDesc
{
    enum ParamType {
        Integer,
    };

    int paramId;
    int defaultValue, maxValue, minValue;
    const char *paramName;
    ParamType paramType;

    PatchParamDesc();
    PatchParamDesc(int id, ParamType type, const char *name, int defaultValue, int minValue, int maxValue);
};

class PatchDescriptor
{
public:
    static const int maxPatchParams = 64;
    bool registerParamInteger(int id, const char *name, int defaultValue, int minValue, int maxValue);
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
