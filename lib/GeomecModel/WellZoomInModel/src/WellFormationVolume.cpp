
#include "WellFormationVolume.h"
#include "WellFormation.h"
#include "WellModelEntryTypes.h"
#include "WellZoomInModel.h"
#include "BodyGroup.h"

CWellFormationVolume::CWellFormationVolume(CWellFormation& formation, geo::CBodyGroup& group)
: CFormationVolume(formation, group)
{
}

TGraphNodeSet CWellFormationVolume::Identifier() const
{
  const CWellFormation& formation = static_cast<const CWellFormation&>(Formation());

  TGraphNodeSet stRet;
  stRet.insert(&const_cast<CFormationBase&>(*formation.ParentFormation()));

  return stRet;
}

int CWellFormationVolume::DisplayListSize() const
{
	int nRet = CFormationVolume::DisplayListSize();
	if(nRet)
  {
		const CWellFormationEntry *pEntry = dynamic_cast<const CWellFormationEntry*>(Model().GraphEntry(MD_WELLMODEL_FORMATION));
		if(pEntry)
		{
			if(pEntry->ShowSkin())
				return 10;
      else if(pEntry->ShowMidpoints())
        return 1;
		}
	}

	return nRet;
}

const geo::IObject& CWellFormationVolume::DisplayList(int nIndex) const
{
	const CWellFormationEntry *pEntry = dynamic_cast<const CWellFormationEntry*>(Model().GraphEntry(MD_WELLMODEL_FORMATION));
	assert(pEntry);

	const CWellZoomInModel& model = static_cast<const CWellZoomInModel&>(Model());
	assert(model.IsMesh());
	
	if(pEntry->ShowSkin())
  {
    const CWellZoomInMesh& mesh = model.Mesh();
    const CWellFormation& formation = static_cast<const CWellFormation&>(Formation());
    const IFormationElementSet& volume = mesh.FormationVolume(formation);
    const geo::CBodyGroup& group = static_cast<const geo::CBodyGroup&>(volume.ElementSet());
    if(group.SideSurfaceSize() > nIndex)
      return group.SideSurface(nIndex);
  }
  else if(pEntry->ShowMidpoints())
  {
    return ElementCenterPoints();
  }

  return CFormationVolume::DisplayList(nIndex);
}
