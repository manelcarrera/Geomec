// IMaterial.h: interface for the IMaterial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMATERIAL_H__838EC4E5_96C9_4493_AC20_85E1005F240B__INCLUDED_)
#define AFX_IMATERIAL_H__838EC4E5_96C9_4493_AC20_85E1005F240B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMaterialBase.h"

#include "MaterialFactory.h"

class CMaterialEntry;
class CLibraryMaterial;
class CMaterialParameter;
class CLibraryMaterialParameter;

#include "mlMaterial.h"

// Broadcasted by modified when param is changed
// #define MAT_PARAM_CHANGED		244

class IMaterial : public IMaterialBase {
  // must be able to get the parameter values from the matlibx (no distribution)
  friend class CFFMaterial;

public:
  class TnoCaseStringCompare {
  public:
    bool operator()(const QString &s1, const QString &s2) const { return s1.compare(s2, Qt::CaseInsensitive) < 0; }
  };

  class TnoCaseQStringCompare {
  public:
    bool operator()(const QString &s1, const QString &s2) const {
      return QString::compare(s1, s2, Qt::CaseInsensitive) < 0;
    }
  };

  IMaterial(CMaterialEntry &entry, CLibraryMaterial &libmat);
  ~IMaterial();

  // Material ....
  virtual unsigned int TypeId() const;
  virtual bool Destroy();
  virtual bool CanDestroy() const;

  // return the parameter value with the given ID, no support for distributed values!
  virtual double ParameterValue(unsigned int ValueTypeID) const;
  double ParameterValue(const QString &MatLibXID) const;

  bool CanExportToMatLib() const;
  void ExportToMatLib();

  // Is locked ....
  bool IsLocked() const;

  void AddMaterialToLibrary(const QString strName, int nModel,
                            std::map<QString, std::pair<double, bool>, TnoCaseStringCompare> mpParam,
                            int nMaxIterations = 20, double dConvCriterion = 0.001);

  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  virtual long SavedItems() const;
  virtual bool Empty() const;

  virtual bool Valid() const;

  // the number of parameters in this material
  size_t ParameterSize() const { return m_mpParameters.size(); }

  // is the given id a valid parameter?
  virtual bool IsParameter(unsigned int ValueTypeID) const;

  virtual bool Less(const CGraphNode &node) const;

  // derived classes must write to filos file
  // this function is declared pure-virtual because it only adds 'default' parameters
  // to the material, but it has to be implemented by a derived class to complete the set
  virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const = 0;
  virtual int MaterialModel() const = 0;
  virtual long MaterialModelFilter() const = 0;

  void Update();

  virtual bool FixedMaterialModel() const;
  void CloneValues(const IMaterial &source_mat);

  //  void ConfigureReadOnlyParametersInLibrary();
  //  void ConfigurePermanentReadOnlyParametersInLibrary();

  const CLibraryMaterial &LibraryMaterial() const;
  CLibraryMaterial &LibraryMaterial();

  // for the material factory
  template <class MATERIAL_TYPE> static IMaterial *Create(CMaterialEntry &entry, CLibraryMaterial &libmat) {
    return new MATERIAL_TYPE(entry, libmat);
  }

  // Interface for dia::IElementProperty -- implementation present
  virtual int Type() const;
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const = 0;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, int i, char *name) const = 0;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, double *values,
                                     int stride = 1) const = 0;

  ACCEPT_GEOMECMODELVISITORS(VisitIMaterial);

protected:
  bool WriteK0(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const;

  int WriteFilosK0ParamSize(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const;
  bool WriteFilosK0ParamName(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, int i, char *name) const;
  void WriteFilosK0ParamValues(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, double *values,
                               int stride = 1) const;

  virtual bool WriteYoungsModulus() const { return true; }
  virtual bool WritePoissonRatio() const { return true; }
  virtual bool WritePermeability() const { return true; }
  virtual bool WriteDefaultPorosity() const { return true; }
  virtual bool WriteDefaultFluidBulkModulus() const { return true; }

private:
  class CObserver : public ml::CMaterial::IObserver {
  public:
    CObserver(IMaterial &parent, ml::CMaterial &notifier);
    virtual ~CObserver();
    virtual void Modified();
    virtual void ForcedUnregister();

  private:
    IMaterial &m_parent;
    ml::CMaterial *m_pNotifier;
  };

  friend class CObserver;

private:
  void CreateParameters();
  void DestroyParameters();
  const CMaterialParameter *Parameter(unsigned int ValueTypeID) const;

  // not allowed...
  IMaterial &operator=(const IMaterial &rhs);

  void LibraryMaterialModified();

private:
  CMaterialEntry &m_entry;
  typedef std::map<CLibraryMaterialParameter *, CMaterialParameter *> TParameterMap;
  TParameterMap m_mpParameters;
  CLibraryMaterial &m_LibraryMaterial;
  CObserver m_observer;
};

typedef CSingleCommandTemplate<IMaterial> TMaterialCommand;

#endif // !defined(AFX_IMATERIAL_H__838EC4E5_96C9_4493_AC20_85E1005F240B__INCLUDED_)
