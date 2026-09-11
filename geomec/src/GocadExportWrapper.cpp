// GocadExportWrapper.cpp: implementation of the CGocadExportWrapper class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Geomec.h"

#include "GocadExportWrapper.h"
#include "ExportGocadDlg.h"
#include "TetraModel.h"
#include "GlobalMessage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGocadExportWrapper::CGocadExportWrapper()
{
}

void CGocadExportWrapper::Export(CModelBase& model, const CResultGroup* pResultGroup)
{
  if(!dynamic_cast<const CTetraModel*>(&model))
  {
  _m()->msg("Gocad export is currently only possible for tetrahedron models");
  return;
  }

  CGocadExport ge(model);
  CExportGocadDlg dlg(ge, pResultGroup);
  dlg.DoModal();
}
