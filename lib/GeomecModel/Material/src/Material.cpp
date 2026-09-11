// Material.cpp: implementation of the CMaterial class.
//
//////////////////////////////////////////////////////////////////////

#include "Material.h"
#include "IMaterial.h"
#include "IMaterialBase.h"
#include "LibraryMaterial.h"
#include "MaterialCreator.h"
#include "MaterialHelperFactory.h"

#include <list>

#include "MaterialErrorHandler.h"
#include "ModelBase.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

namespace {

typedef std::pair<unsigned int, double> TParamValuePair;
typedef std::list<TParamValuePair> TParamValueList;

} // namespace

static bool AssignParameterValues(const TParamValueList &lstParamValue, CLibraryMaterial &mat,
                                  CMaterialErrorHandler &materialErrorHandler) {
  TParamValueList::const_iterator it;
  for (it = lstParamValue.begin(); it != lstParamValue.end(); ++it) {
    QString strErrorMsg;

    mat.ParameterByValueTypeID(it->first)->Value(it->second, strErrorMsg);
    materialErrorHandler.handleError(mat.Name(), mat.ParameterByValueTypeID(it->first)->Name(), strErrorMsg);

    TParamValueList lst;
    TParamValueList::const_iterator itt;
    for (itt = lstParamValue.begin(); itt != lstParamValue.end(); ++itt) {
      if (itt != it)
        lst.push_back(*itt);
    }

    if (AssignParameterValues(lst, mat, materialErrorHandler) || lst.empty()) {
      return true; // success
    }
  }

  // all failed
  return false;
}

static void AssignParameterValuesToGroup(CLibraryMaterial::CGroup &group, const CFFMaterial::TValueMap &mpValue,
                                         CMaterialErrorHandler &materialErrorHandler) {
  TParamValueList lstParamValue;

  for (size_t j = 0; j < group.ParameterSize(); ++j) {
    assert(dynamic_cast<CLibraryMaterialParameter *>(&group.Parameter(j)));
    CLibraryMaterialParameter &param1 = static_cast<CLibraryMaterialParameter &>(group.Parameter(j));
    CFFMaterial::TValueMap::const_iterator it = mpValue.find(param1.ValueTypeID());
    if (it != mpValue.end()) {
      assert(dynamic_cast<CLibraryMaterialParameter *>(&group.Parameter(j)));
      CLibraryMaterialParameter &param2 = static_cast<CLibraryMaterialParameter &>(group.Parameter(j));
      lstParamValue.push_back(std::make_pair(param2.ValueTypeID(), it->second));
      // group.Parameter(j).Value(it->second);
    }
  }

  /*bool bRet =*/AssignParameterValues(lstParamValue, static_cast<CLibraryMaterial &>(group.Material()),
                                       materialErrorHandler);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFFMaterial::CFFMaterial(IMaterialBase &material, const TValueMap &value_map)
    : m_Material(material), m_mpValue(value_map) {
  if (!value_map.empty()) {
    IMaterial *pMat1 = dynamic_cast<IMaterial *>(&m_Material);
    if (pMat1) {
      const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
      ml::CMaterial::CCreator *creator = f->getMatCreator(0); // any will do

      CLibraryMaterial &libmat = pMat1->LibraryMaterial();
      if (libmat.GroupSize() > 0) {
        // group 0 is the 'primary' group
        // if parameters in group 0 are explicitly set by a distribution then
        // those values must be used, any values in other groups may be used
        // if one or more parameter(s) from group 0 can be derived that is/are
        // not explicitly prescribed in a distribution

        // create a clone of the library material
        // this material will be used to get parameter values from
        CLibraryMaterial *pMat2 = libmat.Clone();
        const CModelBase &modelBase = dynamic_cast<const CModelBase &>(Material().Model());
        CMaterialErrorHandler &materialErrorHandler = const_cast<CModelBase &>(modelBase).getMaterialErrorHandler();

        QString errorMessage;
        CLibraryMaterialParameter *libraryMaterialParameter = 0;

        // Assign only if the domain is valid
        for (TValueMap::const_iterator value = m_mpValue.begin(); value != m_mpValue.end(); ++value) {
          libraryMaterialParameter = pMat2->ParameterByValueTypeID((*value).first);
          if (libraryMaterialParameter->CheckDomainValue((*value).second, errorMessage))
            libraryMaterialParameter->ForceValue((*value).second, errorMessage);
          else
            materialErrorHandler.handleError(pMat2->Name(), libraryMaterialParameter->Name(), errorMessage);
        }

        // start with the non-zero groups
        for (int i = 1; i < pMat2->GroupSize(); ++i)
          AssignParameterValuesToGroup(pMat2->Group(i), m_mpValue, materialErrorHandler);

        // now process group 0, this will overwrite any values that were set in
        // the other groups
        AssignParameterValuesToGroup(pMat2->Group(0), m_mpValue, materialErrorHandler);

        // overwrite the values in m_mpValue with those in the cloned material
        m_mpValue.clear();
        for (size_t i = 0; i < pMat2->ParameterSize(); ++i) {
          if (fabs(pMat2->Parameter(i).Value() - libmat.Parameter(i).Value()) > 1e-8)
            m_mpValue.insert(TValueMap::value_type(pMat2->Parameter(i).ValueTypeID(), pMat2->Parameter(i).Value()));
        }

        // delete the cloned material
        creator->Destroy(pMat2);
      }
    }
  }
}

CFFMaterial::~CFFMaterial() {}

void CFFMaterial::SetParameterValue(unsigned int uValueTypeID, double dValue) { m_mpValue[uValueTypeID] = dValue; }

const IMaterialBase &CFFMaterial::Material() const { return m_Material; }

bool CFFMaterial::IsParameter(unsigned int ValueTypeID) const { return m_Material.IsParameter(ValueTypeID); }

double CFFMaterial::ParameterValue(unsigned int ValueTypeID) const {
  TValueMap::const_iterator it = m_mpValue.find(ValueTypeID);
  if (it != m_mpValue.end())
    return it->second;

  return m_Material.ParameterValue(ValueTypeID);
}

bool CFFMaterial::IsDistributedParameter(unsigned int ValueTypeID) const {
  return (m_mpValue.find(ValueTypeID) != m_mpValue.end());
}

bool CFFMaterial::operator<(const dia::IMaterial &rhs) const {
  const CFFMaterial *pRef = dynamic_cast<const CFFMaterial *>(&rhs);

  if (pRef) {
    bool bRet = m_Material.Less(pRef->m_Material);
    if ((!bRet) && (!pRef->m_Material.Less(m_Material))) {
      // Material equal ... so look to map ..
      return std::lexicographical_compare(m_mpValue.begin(), m_mpValue.end(), pRef->m_mpValue.begin(),
                                          pRef->m_mpValue.end());
    }

    return bRet;
  }

  return dia::IMaterial::operator<(rhs);
}

bool CFFMaterial::WriteFilos(dia::IDianaRunner &diarunner) const { return m_Material.Write(*this, diarunner); }

int CFFMaterial::Type() const { return m_Material.Type(); }
int CFFMaterial::WriteFilosParamSize(dia::IDianaRunner &diarunner) const {
  return m_Material.WriteFilosParamSize(*this, diarunner);
}

bool CFFMaterial::WriteFilosParamName(dia::IDianaRunner &diarunner, int i, char *name) const {
  return m_Material.WriteFilosParamName(*this, diarunner, i, name);
}

void CFFMaterial::WriteFilosParamValues(dia::IDianaRunner &diarunner, double *values, int stride) const {
  return m_Material.WriteFilosParamValues(*this, diarunner, values, stride);
}
