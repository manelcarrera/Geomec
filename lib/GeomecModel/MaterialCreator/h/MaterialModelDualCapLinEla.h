#ifndef _MATERIALMODELDUALCAPLINELA_H_
#define _MATERIALMODELDUALCAPLINELA_H_

#include "MaterialCreator.h"

class CMaterialDualCapLinElaCreatorDEPRECATED : public CMaterialCreator {
protected:
  virtual void OnCreateLibrary(CLibraryMaterial &m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual IPQPlotter *PQPlotter() const;
  virtual bool CanCalibrateElastic() const { return true; }
  virtual bool CanCalibratePlastic() const { return true; }
};

class CMaterialDualCapLinElaCreatorV2 : public CMaterialCreator {
protected:
  virtual void OnCreateLibrary(CLibraryMaterial &m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual IPQPlotter *PQPlotter() const;
  virtual bool CanCalibrateElastic() const { return true; }
  virtual bool CanCalibratePlastic() const { return true; }
};

typedef CMaterialDualCapLinElaCreatorV2 CMaterialDualCapLinElaCreator;

class CMaterialDualCapLinElaHardeningCheckStrategy : public ml::CMatParam::CCheckStrategy {
public:
  virtual bool operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialDualCapLinElaSecondaryHardeningCheckStrategy : public ml::CMatParam::CCheckStrategy {
public:
  virtual bool operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg, int nUnitDef = 0) const;
};

#endif // _MATERIALMODELDUALCAPLINELA_H_
