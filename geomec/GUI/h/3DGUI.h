#ifndef _3DGUI_h_
#define _3DGUI_h_

class C3DModel;
class COpenInventorSceneNode;
class OIV2DWellPathGraphScene;
class OIV2DHistoryScene;

#include "GUIBase.h"

class C3DGUI : public CGUIBase
{
public:
  C3DGUI(CFemAppGUI* parentGUI, C3DModel* a3DModel);

  virtual void OnCloseModel();

  virtual void OnCreateSceneEntry();

  C3DModel* model() const;

  OIV2DWellPathGraphScene* getOIV2DWellPathGraphScene() const;
  OIV2DHistoryScene* getOIV2DHistoryScene() const;

  virtual void LoadScenes(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS& progress, bool bIgnore = false);
	virtual void SaveScenes(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS& progress);

private:
  C3DGUI(const C3DGUI& rhs);
  C3DGUI& operator = (const C3DGUI& rhs);

  C3DModel* m_3DModel;
  OIV2DWellPathGraphScene* m_oiv2DWellPathGraphScene;
  OIV2DHistoryScene* m_oiv2DHistoryScene;
};

#endif  // _3DGUI_h_
