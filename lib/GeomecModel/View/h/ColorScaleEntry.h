// ColorScaleEntry.h: interface for the CColorScaleEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORSCALEENTRY_H__FB4457C5_2A92_4063_9F45_EC9AFA2255CA__INCLUDED_)
#define AFX_COLORSCALEENTRY_H__FB4457C5_2A92_4063_9F45_EC9AFA2255CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHotSpot;
class CColorGradient;
class CModelBase;

#include "GeomecModelVisitor.h"
#include "IColorScaleEntry.h"

class CColorScaleEntry : public IColorScaleEntry {
  CColorGradient *m_pLocal;  // The one and only colorscale always available
  CColorGradient *m_pGlobal; // The currently selected color scale
  CHotSpot *m_pHotSpot;      // The currently selected hotspot
public:
  enum COLORSCALE_TYPE { LOCAL, GLOBAL, HOTSPOT };

private:
  COLORSCALE_TYPE m_color_scale;

public:
  // Construction
  CColorScaleEntry(CModelBase &model);
  ~CColorScaleEntry();

  // Set selection
  void SelectHotSpot(CHotSpot &hot_spot);
  void SelectGlobalColorGradient(CColorGradient &gradient);

  // Access to hotspot enz.
  const CHotSpot *HotSpot() const;
  CHotSpot *HotSpot();
  const CColorGradient *GlobalColorGradient() const;
  CColorGradient *GlobalColorGradient();
  const CColorGradient *LocalColorGradient() const;
  CColorGradient *LocalColorGradient();

  // return selected colorgradient
  const CColorGradient *CurrentColorGradient() const;
  CColorGradient *CurrentColorGradient();

  // Selected one
  virtual const IColorScaleNode &CurrentColorScale() const;
  virtual IColorScaleNode &CurrentColorScale();

  // Selecting the gradient
  COLORSCALE_TYPE ColorScaleType() const;
  bool ColorScaleType(const COLORSCALE_TYPE type);

  // Messages
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress);
  void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);
  long SavedItems() const;

  ACCEPT_GEOMECMODELVISITORS(VisitColorScaleEntry);
};

#endif // !defined(AFX_COLORSCALEENTRY_H__FB4457C5_2A92_4063_9F45_EC9AFA2255CA__INCLUDED_)
