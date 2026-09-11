#include "stdafx.h"

#include "FormationVolume_Delegate.h"
#include "FormationVolume.h"
#include "AttriFormationelementset.h"

CFormationVolume_Delegate::CFormationVolume_Delegate(
  CFormationVolume* formationVolume)
: IFormationElementSet_Delegate(formationVolume)
, m_formationVolume(formationVolume)
{
}

bool CFormationVolume_Delegate::Attributes() 
{
  CAttriFormationElementSet <IFormationElementSet> dlg(
    IDD_ATTRI_FORMATION_VOLUME, *m_formationVolume);

  return dlg.DoModal() == IDOK;
}
