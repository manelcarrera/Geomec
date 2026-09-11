#include "stdafx.h"

#include "ResultExportConfiguration.h"

TResultExportConfiguration::TResultExportConfiguration(bool enableAllSelections,
  bool enableExport2LAS)
: enableSlipHorizonSelection(enableAllSelections)
, enableHorizonSelection(enableAllSelections)
, enableFormationSelection(enableAllSelections)
, enableFormationPlaneSelection(enableAllSelections)
, enableRTCISelection(enableAllSelections)
, enableNewWellPathSelection(enableAllSelections)
, enablePointSetSelection(enableAllSelections)
, enableElementSetSelection(enableAllSelections)
, enableNonMeshedSurfaceSelection(enableAllSelections)
, export2LAS(enableExport2LAS)
, selectedNewWellPath(0)
{
}
