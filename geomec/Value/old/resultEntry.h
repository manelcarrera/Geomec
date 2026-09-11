// resultEntry.h: interface for the CresultEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESULTENTRY_H__69C42205_9102_4E50_A6F5_D9CBB42B96CD__INCLUDED_)
#define AFX_RESULTENTRY_H__69C42205_9102_4E50_A6F5_D9CBB42B96CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DepletionStage.h"
#include "Result.h"

class CResultServer;
class CModelBase;
class CProgramResult;
class CResultEntry : public CGraphEntryTemp<IResult>
{
public:
  typedef IValueComposite::TWeightingType TWeightingType;
private:
  typedef std::map<UINT, CProgramResult*> TResultMap;
  TResultMap m_mpResult;
  TWeightingType m_weighting_type;
public:

  CResultServer& m_server;
  void CreateProgramResults();
  void CreateProgramResult(CResultServer::TResultType uResultID,
               UINT uResultName, 
               UINT uExportLabel,
               UINT uBranchName,
               UINT uClassType,
               UINT uSIUnit,
               UINT uFieldUnit,
               const double &dFieldFactor);
public:
  CResultEntry(CResultServer& server);
  virtual ~CResultEntry();

  CResultServer& Server();
  const CResultServer& Server() const;

  const CModelBase& Model() const;
  CModelBase& Model();

  const CProgramResult& Result(UINT uResultID) const;

  void ClearResults();

  TWeightingType WeightingType() const;
  void WeightingType(TWeightingType type);

  virtual void AppendMenu(CMenu& menu);
  virtual void Properties();
  virtual void OnNeighbourModified(const CGraphNode& node, UINT uHint);
  const CProgramResult& Result(CResultServer::TResultType uResultID) const;
};


#endif // !defined(AFX_RESULTENTRY_H__69C42205_9102_4E50_A6F5_D9CBB42B96CD__INCLUDED_)
