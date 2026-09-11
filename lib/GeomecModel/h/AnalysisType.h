#ifndef _ANALYSISTYPE_H_
#define _ANALYSISTYPE_H_

#include "StorageNode.h"

class CAnalysisType {
public:
  typedef CStorageNode::TSTREAM TSTREAM;
  typedef CStorageNode::TPROGRESS TPROGRESS;

  typedef enum _AnalysisType {
    AT_FIRST,
    AT_LINEAR = AT_FIRST,
    AT_NONLIN,
    AT_HEAT,
    AT_MIXTURE,
    AT_MIXTURE_CONTAINMENT,
    AT_LAST = AT_MIXTURE_CONTAINMENT // update when adding types !!
  } TAnalysisType;

  // to iterate over the available analysis types, check for Valid()
  static CAnalysisType FirstType();
  CAnalysisType NextType();

  CAnalysisType();
  CAnalysisType(TAnalysisType antype);
  CAnalysisType(const CAnalysisType &rhs);

  CAnalysisType &operator=(const CAnalysisType &rhs);
  bool operator==(const CAnalysisType &rhs) const;
  bool operator!=(const CAnalysisType &rhs) const;

  CAnalysisType &operator=(TAnalysisType type);
  bool operator==(TAnalysisType type) const;
  bool operator!=(TAnalysisType type) const;

  bool operator<(const CAnalysisType &rhs) const;
  bool operator>(const CAnalysisType &rhs) const;

  bool Valid() const;
  TAnalysisType AnalysisType() const;

  bool FromExportCharacter(char c);
  char ExportCharacter() const;
  QString Label() const;
  unsigned int IconId() const;

  void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  void SaveStream(TSTREAM &stream, TPROGRESS &progress) const;
  long SavedItems() const;

  void LoadStream(std::stringstream &stream, CStreamVersion &version);
  void SaveStream(std::stringstream &stream) const;

  template <class STREAM, bool bStringStream> void LoadStream(STREAM &stream, CStreamVersion &version);

private:
  TAnalysisType m_antype;
  bool m_bValid;
};

#endif // _ANALYSISTYPE_H_
