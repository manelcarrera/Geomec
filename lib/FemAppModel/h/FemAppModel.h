// FemAppModel.h: interface for the CFemAppModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FEMAPPMODEL_H__978CF910_B8C2_46E9_8E9F_F730EC1148D3__INCLUDED_)
#define AFX_FEMAPPMODEL_H__978CF910_B8C2_46E9_8E9F_F730EC1148D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAnalysisLogger;
class CModelProxy;
class CDocumentProxy;

#include "DocumentBase.h"
#include "GraphNode.h"

#define RECALCULATE_ALL 0
#define RECALCULATE_LINEAR 1

class CGraphEntry;
class CGraphNode;
class CFemAppModel : public CGraphNode, public CDocumentBase {
  typedef std::map<int, CGraphEntry *> CEntryMap;
  CEntryMap m_mpEntry;
  friend class CGraphEntry;

protected:
  // ##ModelId=3B653D020021
  CFemAppModel();
  // ##ModelId=3B653D020022
  virtual bool DeleteEntry(const int nEntryType);

public:
  virtual void OnExit();
  virtual void OnCloseModel();
  // Model invalidation ...
  virtual void OnInvalidateStorage();
  virtual void OnInvalidateCalculation(int nType = RECALCULATE_ALL);

  virtual void DestroyCaches();
  virtual void CloseModel();
  // ##ModelId=3B653D020042
  virtual bool NewModel(bool bAttachToDocument = true);

  // ##ModelId=3B653D020050
  virtual CGraphEntry *GraphEntry(const int nEntryType);
  virtual const CGraphEntry *GraphEntry(const int nEntryType) const;

  // This routine only creates the entries for the graph
  virtual void createContainers();
  // This routine creates all default items we want to have.
  virtual void createDefaults();

  // ##ModelId=3B653D020073
  virtual unsigned int ModelIconId() const = 0;
  virtual unsigned int IconId() const { return ModelIconId(); }
  virtual unsigned int TypeId() const { return 0; }
  // ##ModelId=3B653D020081
  CFemAppModel(CAnalysisLogger &logger);

  // ##ModelId=3B653D020083
  virtual ~CFemAppModel();

  virtual int childModelSize() const = 0;
  virtual CFemAppModel &childModel(int index) = 0;
  virtual CFemAppModel *parentModel() = 0;

  enum ModelType {
    e2DModel,
    e3DModel,
    eHexaModel,
    eGm3HexaModel,
    eTetraModel,
    eEclipseModel,
    eGoCadModel,
    eWellModel,
    eWellCasingModel,
    eWellZoomInModel,
    eUndefinedModel
  };

  virtual ModelType modelType() const = 0;

  CAnalysisLogger &Logger() { return m_logger; }
  const CAnalysisLogger &Logger() const { return m_logger; }

  void setModelProxy(CModelProxy *modelProxy);

  CDocumentProxy *getDocumentProxy() const;
  void setDocumentProxy(CDocumentProxy *documentProxy);

  bool getDeleting() const;
  void setDeleting(bool deleting);

  bool getSavingSmoothedResults() const;
  void setSavingSmoothedResults(bool savingSmoothedResults);

  virtual bool Accept(graphnode::IConstVisitor &visitor) const { return visitor.VisitFemAppModel(*this); }
  virtual bool Accept(graphnode::IVisitor &visitor) { return visitor.VisitFemAppModel(*this); }

private:
  CAnalysisLogger &m_logger;
  CModelProxy *m_modelProxy;
  CDocumentProxy *m_documentProxy;
  bool m_deleting;
  bool m_savingSmoothedResults;

  // Document emulation
};

#endif // !defined(AFX_FEMAPPMODEL_H__978CF910_B8C2_46E9_8E9F_F730EC1148D3__INCLUDED_)
