#ifndef _WELLCASINGSTEELMATERIAL_H_
#define _WELLCASINGSTEELMATERIAL_H_

class CModelBase;

#include "IMaterialBase.h"

class CWellCasingSteelMaterialEntry;

class CWellCasingSteelMaterial : public IMaterialBase
{
public:
  class CStressStrain
  {
  public:
    typedef std::pair<double, double> TStressStrainPair;
    typedef std::list<TStressStrainPair> TStressStrainList;
    typedef TStressStrainList::iterator iterator;
    typedef TStressStrainList::const_iterator const_iterator;
    typedef TStressStrainList::reverse_iterator reverse_iterator;
    typedef TStressStrainList::const_reverse_iterator const_reverse_iterator;
    typedef TStressStrainList::size_type size_type;

  public:
    CStressStrain();
    CStressStrain(const CStressStrain& rhs);

    CStressStrain& operator=(const CStressStrain& rhs);
    bool operator==(const CStressStrain& rhs) const;

    bool empty() const;
    size_type size() const;
    void clear();

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    reverse_iterator rbegin();
    reverse_iterator rend();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;

    void push_back(const TStressStrainPair& _Val);
    void push_front(const TStressStrainPair& _Val);
    void pop_back();
    void pop_front();

    iterator insert(iterator _Where, const TStressStrainPair& _Val);

    iterator erase(iterator _Where);
    iterator erase(iterator _First, iterator _Last);

    void sort();

    void LoadStream(CStorageNode::TSTREAM& stream, CStorageNode::TSTREAMVERSION& version, CStorageNode::TPROGRESS& progress);
    void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress);
    long SavedItems() const;

    bool Less(const CStressStrain& rhs) const;

  private:
    class CLess
    {
    public:
      bool operator()(const TStressStrainPair& lhs, const TStressStrainPair& rhs)
      {
        // sort by increasing strain
        return lhs.second < rhs.second;
      }
    };

  private:
    TStressStrainList m_lstStressStrain;
  };

public:
  CWellCasingSteelMaterial(CWellCasingSteelMaterialEntry& entry, const QString& strName);
  CWellCasingSteelMaterial(const CWellCasingSteelMaterial& rhs);

  CWellCasingSteelMaterial& operator=(const CWellCasingSteelMaterial& rhs);
  bool operator==(const CWellCasingSteelMaterial& rhs) const;

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

	virtual double ParameterValue(unsigned int ValueTypeID) const;
	virtual bool IsParameter(unsigned int ValueTypeID) const;
	virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;

  double YoungsModulus() const;
  double PoissonsRatio() const;
  double Density() const;
  double ThermalExpansion() const;
  double ThermalConductivity() const;
  double ThermalCapacity() const;
  bool Perforated() const;

  bool SetYoungsModulus(double dValue);
  bool SetPoissonsRatio(double dValue);
  bool SetDensity(double dValue);
  bool SetThermalExpansion(double dValue);
  bool SetThermalConductivity(double dValue);
  bool SetThermalCapacity(double dValue);
  void SetPerforated(bool bValue);

  CStressStrain& StressStrain();
  const CStressStrain& StressStrain() const;

	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual long SavedItems() const;
  virtual bool Empty() const;
  virtual bool Less(const CGraphNode& node) const;

  // Interface for dia::IElementProperty
  virtual int Type() const;
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitWellCasingSteelMaterial);

private:
  double m_dYoung;
  double m_dPoisson;
  double m_dDensity;
  double m_dThermalExpansion;
  double m_dThermalConductivity;
  double m_dThermalCapacity;
  bool m_bPerforated;

  CStressStrain m_StressStrain;
};

typedef CStorageNode::TSTREAM TSTREAM;
typedef CStorageNode::TPROGRESS TPROGRESS;

class CWellCasingSteelMaterialEntry : public CGraphEntryTemp<CWellCasingSteelMaterial>
{
public:
  CWellCasingSteelMaterialEntry(int nEntryId, CModelBase& model);

	virtual void LoadStream(CFemAppModel& model, TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual long SavedItems() const;
  CWellCasingSteelMaterial& InsertNew();

  ACCEPT_GEOMECMODELVISITORS(VisitWellCasingSteelMaterialEntry);
};

#endif // _WELLCASINGSTEELMATERIAL_H_
