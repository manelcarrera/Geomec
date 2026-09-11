#include "mlMaterial.h"

#include "mlMatParam.h"

#include <cassert>

namespace ml {

CMaterial::CMaterial() : m_pCheckStrategy(0)
{
}

CMaterial::CMaterial(const CMaterial& rhs)
{
  *this = rhs;
}

CMaterial::~CMaterial()
{
  ForceUnregisterObservers();
  DestroyGroups();
  //DestroyParameters();
}

void CMaterial::setCheckStrategy(const CCheckStrategy& checkStrategy)
{
  m_pCheckStrategy = &checkStrategy;
}

bool CMaterial::validateMaterial() const
{
  if (m_pCheckStrategy != 0)
  {
  return (*m_pCheckStrategy) (*this);
  }

  return true;
}

#ifdef OLD_PARAMS
CMaterial* CMaterial::Clone() const
{
  return new CMaterial(*this);
}
#endif

CMaterial& CMaterial::operator=(const CMaterial& rhs)
{
  m_strName        = rhs.m_strName;
  m_nMaterialModel = rhs.m_nMaterialModel;
  m_strModelName   = rhs.m_strModelName;

#ifdef OLD_N2P
  m_mpName2Param.clear();
#endif
  DestroyGroups();
  DestroyParameters();

  CopyParameters(rhs);

#ifdef OLD_PARAMS
  m_vcParams.resize(rhs.m_vcParams.size());
  size_t i;
  for(i = 0; i < rhs.m_vcParams.size(); ++i)
  {
  m_vcParams[i] = rhs.m_vcParams[i]->Clone(*this);
  }
#endif

  m_vcGroups.reserve(rhs.m_vcGroups.size());
  for(size_t i = 0; i < rhs.m_vcGroups.size(); ++i)
  {
  m_vcGroups.push_back(CGroup(this, rhs.m_vcGroups[i].Name()));
  m_vcGroups[i].m_vcParameters = rhs.m_vcGroups[i].m_vcParameters;
  assert(m_vcGroups[i].m_vcParameters.size() == m_vcGroups[i].m_vcParameters.size());
  }

  m_vcParam2Group = rhs.m_vcParam2Group;

  m_pCheckStrategy = rhs.m_pCheckStrategy;

  Modified();

  return *this;
}

bool CMaterial::operator==(const CMaterial& rhs) const
{
  if(m_strName        != rhs.m_strName)
  return false;
  if(m_nMaterialModel != rhs.m_nMaterialModel)
  return false;
  if(m_strModelName   != rhs.m_strModelName)
  return false;

  assert(ParameterSize() == rhs.ParameterSize());
  size_t i;
  for(i = 0; i < ParameterSize(); ++i)
  {
  if(MatParameter(i) != rhs.MatParameter(i))
      return false;
  }

  return true;
}

bool CMaterial::operator!=(const CMaterial& rhs) const
{
  return !operator==(rhs);
}

const QString& CMaterial::Name() const
{
  return m_strName;
}

void CMaterial::Name(const QString& strName)
{
  m_strName = strName;
  Modified();
}

int CMaterial::MaterialModel() const
{
  return m_nMaterialModel;
}

const QString& CMaterial::MaterialModelName() const
{
  return m_strModelName;
}

#ifdef OLD_PARAMS
int CMaterial::ParameterSize() const
{
  return int(m_vcParams.size());
}

const CMatParam& CMaterial::MatParameter(size_t i) const
{
  assert(i >= 0 && i < m_vcParams.size());
  return *m_vcParams[i];
}

CMatParam& CMaterial::MatParameter(size_t i)
{
  assert(i >= 0 && i < m_vcParams.size());
  return *m_vcParams[i];
}
#endif

const CMatParam* CMaterial::MatParameter(const QString& strName) const
{
  return (const_cast<CMaterial*>(this))->MatParameter(strName);
}

CMatParam* CMaterial::MatParameter(const QString& strName)
{
#ifdef OLD_N2P
  if(m_mpName2Param.empty())
  CreateName2ParamMap();

  TName2ParamMap::const_iterator it = m_mpName2Param.find(strName);
  if(it != m_mpName2Param.end())
  return it->second;
#else
  for (size_t i = 0; i < ParameterSize(); ++i)
  {
  if (MatParameter(i).Name() == strName)
      return &MatParameter(i);
  }
#endif

  return 0;
}

int CMaterial::GroupSize() const
{
  return int(m_vcGroups.size());
}

const CMaterial::CGroup& CMaterial::Group(size_t i) const
{
  // comparison of unsigned expression >= 0 is always true

  assert(/*i >= 0 &&*/ i < m_vcGroups.size());
  return m_vcGroups[i];
}

CMaterial::CGroup& CMaterial::Group(size_t i)
{
  // comparison of unsigned expression >= 0 is always true

  assert(/*i >= 0 &&*/ i < m_vcGroups.size());
  return m_vcGroups[i];
}

const CMaterial::CGroup* CMaterial::ParameterGroup(const CMatParam& param) const
{
#ifdef OLD_P2G
  TParam2GroupMap::const_iterator it = m_mpParam2Group.find(&param);
  if(it != m_mpParam2Group.end())
  return it->second;
#else
  for (size_t i = 0; i < ParameterSize(); ++i)
  {
  if (&MatParameter(i) == &param)
      return m_vcParam2Group[i] >= 0 ? &m_vcGroups[m_vcParam2Group[i]] : 0;
  }
#endif

  return 0;
}

CMaterial::CGroup* CMaterial::ParameterGroup(CMatParam& param)
{
#ifdef OLD_P2G
  TParam2GroupMap::const_iterator it = m_mpParam2Group.find(&param);
  if(it != m_mpParam2Group.end())
  return it->second;
#else
  for (size_t i = 0; i < ParameterSize(); ++i)
  {
  if (&MatParameter(i) == &param)
      return m_vcParam2Group[i] >= 0 ? &m_vcGroups[m_vcParam2Group[i]] : 0;
  }
#endif
  return 0;
}

void CMaterial::SwitchMaterialModel(CCreator& creator, bool bKeepParamValues)
{
  typedef std::map<QString, double> TNameValueMap;
  TNameValueMap mpNameValue;

  if(bKeepParamValues)
  {
  for(size_t i = 0; i < ParameterSize(); ++i)
      mpNameValue.insert(TNameValueMap::value_type(MatParameter(i).Name(), MatParameter(i).Value()));
  }

#ifdef OLD_N2P
  m_mpName2Param.clear();
#endif
  DestroyGroups();
  DestroyParameters();
  assert(ParameterSize() == 0);
  creator.OnCreateParameters(*this);
  assert(ParameterSize() > 0);

  if(bKeepParamValues)
  {
  for(size_t i = 0; i < ParameterSize(); ++i)
  {
      TNameValueMap::iterator it = mpNameValue.find(MatParameter(i).Name());
      if(it != mpNameValue.end())
      {
    if(MatParameter(i).CheckValue(it->second))
          MatParameter(i).LoadValue(it->second);
      }
  }

  Finalize();
  }

  m_nMaterialModel = creator.MaterialModel();
  m_strModelName = creator.MaterialModelName();

  Modified();
}

void CMaterial::RegisterObserver(IObserver& observer)
{
  // unless we have a lot of observers, it's overkill to sort (which would make this identical to std::set)
  if (std::find(m_vcObservers.begin(), m_vcObservers.end(), &observer) == m_vcObservers.end())
  m_vcObservers.push_back(&observer);
}

void CMaterial::UnregisterObserver(IObserver& observer)
{
  TObserverVector::iterator it = std::find(m_vcObservers.begin(), m_vcObservers.end(), &observer);
  if (it != m_vcObservers.end())
  m_vcObservers.erase(it);
}

#ifdef OLD_N2P
void CMaterial::CreateName2ParamMap() const
{
  assert(m_mpName2Param.empty());
  size_t i;
  for(i = 0; i < m_vcParams.size(); ++i)
  m_mpName2Param.insert(TName2ParamMap::value_type(m_vcParams[i]->Name(), m_vcParams[i]));
}
#endif

#ifdef OLD_PARAMS
void CMaterial::DestroyParameters()
{
  size_t i;
  for(i = 0; i < m_vcParams.size(); ++i)
  delete m_vcParams[i];
  m_vcParams.clear();
}
#endif

void CMaterial::DestroyGroups()
{
#ifdef OLD_P2G
  m_mpParam2Group.clear();
#else
  m_vcParam2Group.clear();
#endif
  //size_t i;
  //for(i = 0; i < m_vcGroups.size(); ++i)
  //delete m_vcGroups[i];
  m_vcGroups.clear();
}

void CMaterial::Finalize()
{
#ifdef _WIN32
  static __declspec(thread) bool bLockRecursion = false;
#else
  static bool bLockRecursion = false;
#endif
  if(bLockRecursion)
  return;

  bLockRecursion = true;

  // finalize parameter defaults
  size_t i;
  for(i = 0; i <ParameterSize(); ++i)
  {
  // trigger dependencies
  QString s;
  MatParameter(i).ForceValue(MatParameter(i).Value(), s);
//    assert(bRet); // must be a valid default
  }

  bLockRecursion = false;
}

void CMaterial::Modified()
{
  TObserverVector vcObservers = m_vcObservers; // this might be deleted...
  TObserverVector::iterator it;
  for(it = vcObservers.begin(); it != vcObservers.end(); ++it)
  (*it)->Modified();
}

void CMaterial::ForceUnregisterObservers()
{
  for (TObserverVector::iterator it = m_vcObservers.begin(); it != m_vcObservers.end(); ++it)
  (*it)->ForcedUnregister();
  m_vcObservers.clear();
}


///// CMaterial::CGroup
CMaterial::CGroup::CGroup(CMaterial* mat, const QString& strName)
: m_mat(mat),
  m_strName(strName)
{
}

const QString& CMaterial::CGroup::Name() const
{
  return m_strName;
}

size_t CMaterial::CGroup::ParameterSize() const
{
  return m_vcParameters.size();
}

const CMatParam& CMaterial::CGroup::Parameter(size_t i) const
{
  // comparison of unsigned expression >= 0 is always true

  assert(/*i >= 0 &&*/ i < m_vcParameters.size());
  return m_mat->MatParameter(m_vcParameters[i]);
}

CMatParam& CMaterial::CGroup::Parameter(size_t i)
{
  // comparison of unsigned expression >= 0 is always true

  assert(/*i >= 0 &&*/ i < m_vcParameters.size());
  return m_mat->MatParameter(m_vcParameters[i]);
}

void CMaterial::CGroup::AddParameter(size_t parent_i)
{
#if 0
  // this doesn't work, we reference parameters before they exist?
#ifdef _DEBUG
  for(size_t i = 0; i < m_vcParameters.size(); ++i)
  assert(MatParameter(i).Name() != m_mat->MatParameter(parent_i).Name());
#endif
#endif

  m_vcParameters.push_back(parent_i);
}

CMaterial& CMaterial::CGroup::Material()
{
  assert(m_mat != 0);

  return *m_mat;
}

const CMaterial& CMaterial::CGroup::Material() const
{
  assert(m_mat != 0);

  return *m_mat;
}


///// CMaterial::CCreator

CMaterial::CCreator::~CCreator()
{
}

CMaterial::CCreator::CCreator()
{
}

void CMaterial::CCreator::Add(CMatParam* pParam, const QString& strGroupName)
{
  CMaterial& mat = pParam->ParentMaterial();

  // add to material
  size_t param_index = mat.AddParameter(pParam);

#ifndef OLD_P2G
  mat.m_vcParam2Group.push_back(-1);
#endif

  if(!strGroupName.isEmpty()) // named group
  {
#ifndef OLD_P2G
  long long group_index = -1;
#endif

  // does group already exist?
  //CGroup* pGroup = 0;
  size_t i;
  for(i = 0; i < mat.m_vcGroups.size(); ++i)
  {
      if(mat.m_vcGroups[i].Name() == strGroupName)
      {
    //pGroup = mat.m_vcGroups[i];
#ifndef OLD_P2G
    group_index = i;
#endif
    break;
      }
  }

  if(group_index < 0)
  {
      // create new group
      //pGroup = new CGroup(mat, strGroupName);
#ifndef OLD_P2G
      group_index = mat.m_vcGroups.size();
#endif
      mat.m_vcGroups.push_back(CGroup(&mat, strGroupName));

      // add ungrouped parameters we've already encountered to this group also
      for(i = 0; i < m_vcUngrouped.size(); ++i)
    mat.m_vcGroups[group_index].AddParameter(m_vcUngrouped[i]);
  }

  // add this parameter to the group
  mat.m_vcGroups[group_index].AddParameter(param_index);

  // register the group name with the parameter
#ifdef OLD_P2G
  mat.m_mpParam2Group.insert(CMaterial::TParam2GroupMap::value_type(pParam, pGroup));
#else
  assert(group_index >= 0);
  assert(param_index < mat.m_vcParam2Group.size());
  assert(mat.m_vcParam2Group[param_index] == -1);
  mat.m_vcParam2Group[param_index] = group_index;
#endif
  }
  else // unnamed
  {
  // add to all groups
  size_t i;
  for(i = 0; i < mat.m_vcGroups.size(); ++i)
      mat.m_vcGroups[i].AddParameter(param_index);

  // add to vector with ungrouped parameters, so can be added to new groups later
  m_vcUngrouped.push_back(param_index);
  }
}

#ifdef OLD_PARAMS
CMaterial* CMaterial::CCreator::OnCreateMaterial()
{
  return new CMaterial;
}
#endif

void CMaterial::CCreator::Create(const QString& strName, CMaterial& mat)
{
  OnCreateParameters(mat);
  mat.m_nMaterialModel = MaterialModel();
  mat.m_strModelName = MaterialModelName();
  mat.Name(strName);

  mat.Finalize();
}

void CMaterial::CCreator::Destroy(CMaterial* mat)
{
  delete mat;
}

void CMaterial::CCreator::Reset()
{
  m_vcUngrouped.clear();
}


} // namespace ml
