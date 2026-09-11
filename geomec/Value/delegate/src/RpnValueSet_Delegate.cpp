#include "stdafx.h"

#include "RpnValueSet_Delegate.h"
#include "RpnValueSet.h"
#include "RpnDlg.h"
#include "ModelBase.h"
#include "RpnFormationOperand.h"
#include "resourceIDS.h"

CRpnValueSet_Delegate::CRpnValueSet_Delegate(CRpnValueSet* rpnValueSet)
: IValueSet_Delegate(rpnValueSet)
, m_rpnValueSet(rpnValueSet)
{
}

bool CRpnValueSet_Delegate::Attributes()
{
  // Create a stack if we don't have any ..

  if (m_rpnValueSet->getStack() == 0)
  {
  m_rpnValueSet->setStack(new rpn::CRpnStack());

  // Insert X, Y, Z coordinates

  CModelBase& model = dynamic_cast <CModelBase&> (m_rpnValueSet->Model());

  new CRpnValueSet::CValueSetCoordinateProxy(*m_rpnValueSet,
      IDS_RC_NORTHING, *m_rpnValueSet->getStack(),
      CRpnValueSet::CValueSetCoordinateProxy::CO_X);
  new CRpnValueSet::CValueSetCoordinateProxy(*m_rpnValueSet,
      IDS_RC_EASTING, *m_rpnValueSet->getStack(),
      CRpnValueSet::CValueSetCoordinateProxy::CO_Y);
  new CRpnValueSet::CValueSetCoordinateProxy(*m_rpnValueSet,
      IDS_RC_DEPTH, *m_rpnValueSet->getStack(),
      CRpnValueSet::CValueSetCoordinateProxy::CO_Z);

  // Insert an IsReservoir

  new CRpnReservoirProxy(model.Mesh(), *m_rpnValueSet->getStack());
  }

  m_rpnValueSet->UpdateProxy();

  // Launch the dialoque

  CRpnDlg dlg(*m_rpnValueSet);
  bool dlgIsOK = (dlg.DoModal() == IDOK);

  if (dlgIsOK)
  {
  m_rpnValueSet->PointSet().Modified();
  }

  return dlgIsOK;
}

bool CRpnValueSet_Delegate::CanDestroy() const
{
  return m_rpnValueSet->CanDestroy();
}
