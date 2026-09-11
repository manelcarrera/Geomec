#ifndef _GVT_ANALYSIS_H_
#define _GVT_ANALYSIS_H_

class IProgressBase;
class CGVTData;
class CModelBase;
class CPointSet;

class CGVTAnalysis {
public:
  CGVTAnalysis();

  void Calculate(IProgressBase &progress, CGVTData &data);
};

#endif