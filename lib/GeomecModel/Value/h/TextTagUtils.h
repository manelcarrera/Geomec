#ifndef TEXTTAGUTILS_H
#define TEXTTAGUTILS_H
#include <string>

class CModelBase;
class CDepletionStage;
class CAnalysisType;

const CAnalysisType AnalysisType(const std::string &atype);

const std::string StripQuotes(const std::string &string);

std::string VectorResultComponentList();
std::string TensorResultComponentList();
std::string StressInvariantResultComponentList();
std::string StrainInvariantResultComponentList();
std::string WellPathStressComponentList();
std::string SurfaceStressComponentList();

bool SimpleSplitTextTag(const std::string &textTag, std::string &baseTag, int &timeStepIndex);

bool SplitTextTag(const std::string &textTag, const std::string &componentList, const std::string &analysisTypeList,
                  std::string &baseTag, std::string &component, int &timeStepIndex, std::string &analysisType);
#endif // TEXTTAGUTILS_H
