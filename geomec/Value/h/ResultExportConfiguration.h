#ifndef _ResultExportConfiguration_h_
#define _ResultExportConfiguration_h_

class CNewWellPath;

struct TResultExportConfiguration {
  TResultExportConfiguration(bool enableAllSelections = true, bool enableExport2LAS = false);

  bool enableSlipHorizonSelection;
  bool enableHorizonSelection;
  bool enableFormationSelection;
  bool enableFormationPlaneSelection;
  bool enableRTCISelection;
  bool enableNewWellPathSelection;
  bool enablePointSetSelection;
  bool enableElementSetSelection;
  bool enableNonMeshedSurfaceSelection;
  bool export2LAS;
  CNewWellPath *selectedNewWellPath;
};

#endif // _ResultExportConfiguration_h_
