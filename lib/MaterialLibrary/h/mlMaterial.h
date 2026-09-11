#ifndef _MLMATERIAL_H_
#define _MLMATERIAL_H_

#include <vector>
#include <map>
#include <set>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>

#ifdef TEST_MATERIALS
namespace test_materials
{
  class AccessMaterial;
}
#endif


namespace ml {

class CMatParam;

//! The material owns its parameters
class CMaterial
{
public:
  class CGroup
  {
  public:
  CGroup(CMaterial* mat, const QString& strName);

  const QString& Name() const;

  size_t ParameterSize() const;
  const CMatParam& Parameter(size_t i) const;
  CMatParam& Parameter(size_t i);

  void AddParameter(size_t parent_i);

  CMaterial& Material();
  const CMaterial& Material() const;

  private:
  CMaterial* m_mat;
  QString m_strName;
  protected:
  friend class CMaterial;
  std::vector<size_t> m_vcParameters;
  };

  class CCreator
  {
  protected:
  CCreator();
  virtual ~CCreator();
  public:

  // must call this function from OnCreateParameters, pParam must be created on the heap (with new)
  void Add(CMatParam* pParam, const QString& strGroupName = QString());

  virtual CMaterial* OnCreateMaterial() = 0; // CMaterial is now abstract
  virtual void OnCreateParameters(CMaterial& mat) = 0;
  virtual int MaterialModel() = 0;
  virtual QString MaterialModelName() = 0;
  void Create(const QString& strName, CMaterial& mat);

  void Destroy(CMaterial* mat);

  void Reset();

  private:
  std::vector<size_t> m_vcUngrouped;
  };

  friend class CCreator;

  class IObserver
  {
  public:
  virtual ~IObserver() {}
  virtual void Modified() = 0;
  virtual void ForcedUnregister() = 0;
  };

  class CCheckStrategy
  {
  public:
  CCheckStrategy() {}
  virtual ~CCheckStrategy() {}
  virtual bool operator () (const ml::CMaterial& material) const = 0;
  private:
  CCheckStrategy(const CCheckStrategy& rhs);
  CCheckStrategy& operator = (const CCheckStrategy& rhs);
  };

public:
  void setCheckStrategy(const CCheckStrategy& checkStrategy);
  bool validateMaterial() const;

  virtual CMaterial* Clone() const = 0;

  CMaterial& operator=(const CMaterial& rhs);
  bool operator==(const CMaterial& rhs) const;
  bool operator!=(const CMaterial& rhs) const;

  const QString& Name() const;
  void Name(const QString& strName);

  virtual int MaterialModel() const;
  virtual const QString& MaterialModelName() const;

  virtual size_t ParameterSize() const = 0;

  // by index
  virtual const CMatParam& MatParameter(size_t i) const = 0;
  virtual CMatParam& MatParameter(size_t i) = 0;

  // by name
  virtual const CMatParam* MatParameter(const QString& strName) const;
  virtual CMatParam* MatParameter(const QString& strName);

  virtual int GroupSize() const;
  virtual const CGroup& Group(size_t i) const;
  virtual CGroup& Group(size_t i);

  virtual const CGroup* ParameterGroup(const CMatParam& param) const;
  virtual CGroup* ParameterGroup(CMatParam& param);

  virtual void SwitchMaterialModel(CCreator& creator, bool bKeepParamValues);

  void RegisterObserver(IObserver& observer);
  void UnregisterObserver(IObserver& observer);

  void Finalize();

protected:
  CMaterial();
  CMaterial(const CMaterial& rhs);

  friend class CMaterialLibrary;
  virtual ~CMaterial();

  virtual void DestroyParameters() = 0;
  virtual void CopyParameters(const CMaterial& rhs) = 0;
  virtual size_t AddParameter(CMatParam *param) = 0;

private:
#ifdef OLD_N2P
  void CreateName2ParamMap() const;
#endif
  void DestroyGroups();

  void Modified();
  void ForceUnregisterObservers();

private:
  QString m_strName;
  int m_nMaterialModel;
  QString m_strModelName;
  //std::vector<CMatParam*> m_vcParams;

  std::vector<CGroup> m_vcGroups;

#ifdef OLD_N2P
  // possibly premature optimalization due to limited number of parameters
  // if we find it IS necessary, we'll try QHash or something similar
  typedef std::map<QString, CMatParam*> TName2ParamMap;
  mutable TName2ParamMap m_mpName2Param;
#endif

#ifdef OLD_P2G
  typedef std::map<const CMatParam*, CGroup*> TParam2GroupMap;
  TParam2GroupMap m_mpParam2Group;
#else
  typedef std::vector<long long> TParam2GroupVector;
  TParam2GroupVector m_vcParam2Group;
#endif

  typedef std::vector<IObserver*> TObserverVector;
  TObserverVector m_vcObservers;

#ifdef TEST_MATERIALS
  friend class test_materials::AccessMaterial;
#endif
  const CCheckStrategy *m_pCheckStrategy;
};

typedef CMaterial* TMaterialPtr;
typedef CMaterial::CCreator* TMaterialCreatorRefPtr;

} // namespace ml

#endif // _MLMATERIAL_H_
