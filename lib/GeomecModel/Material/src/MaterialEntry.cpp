// MaterialEntry.cpp: implementation of the CMaterialEntry class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseEntryTypes.h"
#include "MaterialEntry.h"
#include "ModelBase.h"
#include "KeyFile.h"
#include "BranchState.h"
#include "MaterialLibraryXML.h"
#include "mlxmlException.h"
#include "LibraryMaterial.h"
#include "mlxmlMaterialLibrary.h"
#include "GlobalMessage.h"
#include "MaterialHelperFactory.h"
#include "Materials.h"
#include "resourceIDP.h"

CMaterialEntry::CMaterialEntry(int nEntryId, unsigned int uIconId, const QString& strName, CModelBase& model)
: CGraphEntryTemp<IMaterial>(nEntryId, uIconId, strName, model),
  m_pMaterialLibrary(new ml::CMaterialLibrary),
  m_pLibraryObserver(new CMaterialLibraryObserver(*this)),
  m_pExternalLibrary(0)
{
}

CMaterialEntry::CMaterialEntry(int nEntryId, unsigned int uIconId, unsigned int uName, CModelBase& model)
: CGraphEntryTemp<IMaterial>(nEntryId, uIconId, uName, model),
  m_pMaterialLibrary(new ml::CMaterialLibrary),
  m_pLibraryObserver(new CMaterialLibraryObserver(*this)),
  m_pExternalLibrary(0)
{
}

/*
void CMaterialEntry::SetFilterRockMaterial(CMatLib& library) TODO wedx
{
	int nFilter =
		MLFO_LINEAR              |
		MLFO_MOHRCOULOMB         |
		MLFO_MODIFIEDMOHRCOULOMB |
		MLFO_MC_COHESION_HARD1   |
		MLFO_MC_COHESION_HARD2   |
		MLFO_MC_COHESION_HARD3   |
		MLFO_MC_FRICTION_HARD1   |
		MLFO_MC_FRICTION_HARD2   |
		MLFO_MC_FRICTION_HARD3	 |
		MLFO_RIGIDITY            |
    MLFO_DUALCAP_LINELA      |
    MLFO_FRACTURE_ANISOTROPY |
    MLFO_UPSCALED_ANISOTROPY |
    MLFO_ANISOTROPIC_CAMCLAY;

	// only allow undrained material model if not 2D (mantis #1442)
	const C2DModel* p2DModel = dynamic_cast<const C2DModel*> (&Model());
	if(!p2DModel) nFilter |= MLFO_UNDRAINED;

	CKeyFile keyFile;

#ifdef CAMCLAY_KEYFILE
	if(keyFile.Unlocked())
	{
#endif
		nFilter |= MLFO_CAMCLAY;
#ifdef CAMCLAY_KEYFILE
	}
#endif

#ifdef FOKKER_KEYFILE
	if(keyFile.Unlocked())
	{
#endif
		nFilter |= MLFO_CREEP;
#ifdef FOKKER_KEYFILE
	}
#endif

#ifdef FRACTURE_APERTURE_KEYFILE
  if(keyFile.Unlocked())
  {
#endif
    nFilter |= MLFO_FRACTURE_APERTURE;
#ifdef FRACTURE_APERTURE_KEYFILE
  }
#endif

	library.SetMaterialModelFilter(nFilter);
}
*/
CMaterialEntry::~CMaterialEntry()
{
  delete m_pLibraryObserver;
  delete m_pMaterialLibrary;
  delete m_pExternalLibrary;
}

void CMaterialEntry::ExportToMatLib(const CLibraryMaterial& mat)
{
  CLibraryMaterial* pClone = mat.Clone();
  m_pExternalLibrary->AddMaterial(*pClone);
  UpdateExternalMatLib();
}

void CMaterialEntry::ImportFromMatLib(const CLibraryMaterial& mat)
{
  CLibraryMaterial* pClone = mat.Clone();
  m_pMaterialLibrary->AddMaterial(*pClone);
}

void CMaterialEntry::SetExternalMatLibFileName(const QString& strFileName, bool bImport)
{
  m_strExternalLibraryFileName = strFileName;
  if(bImport)
    ReadExternalMatLib();
  else
  {
    if(!m_pExternalLibrary)
      m_pExternalLibrary = new ml::CMaterialLibrary;
    else
      m_pExternalLibrary->Clear();
    UpdateExternalMatLib();
  }
}

const QString& CMaterialEntry::GetExternalMatLibFileName() const
{
  return m_strExternalLibraryFileName;
}

bool CMaterialEntry::HasValidMatLib() const
{
  return !m_strExternalLibraryFileName.isEmpty();
}

void CMaterialEntry::ReadExternalMatLib()
{
  assert(!m_strExternalLibraryFileName.isEmpty());

  if(!m_pExternalLibrary)
    m_pExternalLibrary = new ml::CMaterialLibrary;

  m_pExternalLibrary->Clear();
  CMaterialLibraryXML matlibxml(*m_pExternalLibrary);
  try
  {
    matlibxml.LoadFile(m_strExternalLibraryFileName);
  }
  catch(mlxml::CException e)
  {
    _m()->status(QObject::tr("Unable to load the external material library: %1").arg(e.error()));
    // not working, reset
    m_strExternalLibraryFileName.clear();
  }
}

void CMaterialEntry::UpdateExternalMatLib()
{
  assert(!m_strExternalLibraryFileName.isEmpty());
  assert(m_pExternalLibrary);

  CMaterialLibraryXML matlibxml(*m_pExternalLibrary);
  try
  {
    matlibxml.SaveFile(m_strExternalLibraryFileName);
  }
  catch(mlxml::CException e)
  {
    _m()->msg(QObject::tr("Unable to save the external material library:\n\n%1").arg(e.error()));
  }
}

IMaterial& CMaterialEntry::CreateMaterial(int iMaterialModel, const QString& strName)
{
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  ml::CMaterial::CCreator* pCreator = f->getMatCreator(iMaterialModel);
  assert(pCreator);

  ml::CMaterial& matbase = m_pMaterialLibrary->AddMaterial(strName, *pCreator);

  assert(dynamic_cast<CLibraryMaterial*>(&matbase));
  CLibraryMaterial& mat = static_cast<CLibraryMaterial&>(matbase);

  TMaterialMap::iterator it = m_mpMaterial.find(&mat);
  assert(it != m_mpMaterial.end());
  IMaterial* pMat = it->second;
  assert(pMat);

  return *pMat;
}

void CMaterialEntry::DeleteMaterial(IMaterial& material)
{
  m_pMaterialLibrary->DeleteMaterial(material.LibraryMaterial());
}

IMaterial& CMaterialEntry::ConvertMaterial(IMaterial& mat, int iNewMaterialModel)
{
  CLibraryMaterial& libmat = mat.LibraryMaterial();

  // create the new material
  IMaterial& newmat = CreateMaterial(iNewMaterialModel, libmat.Name());
  CLibraryMaterial& newlibmat = newmat.LibraryMaterial();

  // set the parameter values
  size_t i;
  for(i = 0; i < libmat.ParameterSize(); ++i)
  {
    const CLibraryMaterialParameter& oldparam = libmat.Parameter(i);
    CLibraryMaterialParameter* pNewParam = newlibmat.Parameter(oldparam.Name());
    if(pNewParam)
      pNewParam->Value(oldparam.Value());
  }

  // remove the old material
  m_pMaterialLibrary->DeleteMaterial(libmat);
  assert(m_mpMaterial.find(&libmat) == m_mpMaterial.end());

  return newmat;
}

void CMaterialEntry::LoadStream(CFemAppModel& model, TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	// Get Materials
	stream.Flush();

	int nMaterialSize;
	stream >> nMaterialSize;

  bool bLockedFractureApertureMessageShown = false;

  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();

	for(int i = 0; i < nMaterialSize; i++)
	{
		int nMaterialModel;
		stream >> nMaterialModel;

		// create new material based on model and load it
    ml::CMaterial::CCreator* pCreator1 = f->getMatCreator(nMaterialModel);
    assert(pCreator1);
    ml::CMaterial& libmat1 = m_pMaterialLibrary->AddMaterial("", *pCreator1);

    assert(dynamic_cast<CLibraryMaterial*>(&libmat1));
		IMaterial *pMat = CorrespondingMaterial(static_cast<CLibraryMaterial&>(libmat1));
		assert(pMat);
    assert(isChild(*pMat) >= 0);
		pMat->LoadStream(stream, version, progress);

    // dual cap material model has been removed (hidden)
    if(nMaterialModel == MM_DUALCAP)
    {
      m_pMaterialLibrary->DeleteMaterial(libmat1);
			CModelBase &modelbase = static_cast<CModelBase&>(model);
      modelbase.InvalidateResultsAfterLoad();
    }

		CKeyFile keyFile;

#ifdef CAMCLAY_KEYFILE
		if(!keyFile.Unlocked() && nMaterialModel == MM_CAMCLAY)
		{
			CModelBase &modelbase = dynamic_cast<CModelBase &> (model);
			if(!bLockedCamClayMessageShown)
			{
				_m()->msg(IDP_LOAD_CAMCLAY_LOCKED);
				bLockedCamClayMessageShown = true;
			}

			UnLink(*pMat);
			delete pMat;
			modelbase.InvalidateResultsAfterLoad();
		}
#endif

#ifdef FOKKER_KEYFILE
		if(!keyFile.Unlocked() && nMaterialModel == MM_CREEP)
		{
			CModelBase &modelbase = dynamic_cast<CModelBase &> (model);
			if(!bLockedFokkerMessageShown)
			{
				_m()->msg(IDP_LOAD_FOKKER_LOCKED);
				bLockedFokkerMessageShown = true;
			}

			UnLink(*pMat);
			delete pMat;
			modelbase.InvalidateResultsAfterLoad();
		}
#endif

		if(nMaterialModel == MM_FRACTURE_APERTURE)
		{
			CModelBase &modelbase = dynamic_cast<CModelBase &> (model);
			if(!bLockedFractureApertureMessageShown)
			{
				_m()->msg("At least one formation makes use of the obsolete 'Fracture Aperture' material model, these materials have been changed into the new 'Fracture Aperture 2' material model");
				bLockedFractureApertureMessageShown = true;
			}

      CLibraryMaterial& libmat2 = pMat->LibraryMaterial();

      // do the switch
      ml::CMaterial::CCreator* pCreator2 = f->getMatCreator(MM_FRACTURE_APERTURE2);
      libmat2.SwitchMaterialModel(*pCreator2, true);

			modelbase.InvalidateResultsAfterLoad();
		}

	}
}

void CMaterialEntry::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
	TNodeSet stNodes = EntryNodes();	
	int nNodeSize = stNodes.size();
	stream << nNodeSize;

	// Store each material
	int nIndex = 1;
	for (TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); it++)
	{
		// Save material model
		stream << (*it)->MaterialModel();
		(*it)->Index(nIndex);
		(*it)->SaveStream(stream, progress);
		nIndex++;
	}
}

long CMaterialEntry::SavedItems() const
{
	long lRet = 0;
	TNodeSet stNode = EntryNodes();
	for(TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
	{
		lRet += (*it)->SavedItems();
	}
	return lRet;
}

int CMaterialEntry::MaterialSize() const
{
  return m_pMaterialLibrary->MaterialSize();
}

const IMaterial& CMaterialEntry::Material(int i) const
{
  return (const_cast<CMaterialEntry*>(this))->Material(i);
}

IMaterial& CMaterialEntry::Material(int i)
{
  assert(dynamic_cast<CLibraryMaterial*>(&m_pMaterialLibrary->Material(i)));
  TMaterialMap::iterator it = m_mpMaterial.find(static_cast<CLibraryMaterial*>(&m_pMaterialLibrary->Material(i)));
  assert(it != m_mpMaterial.end());
  return *it->second;
}

bool CMaterialEntry::ReadOnly() const
{
	// material is read-only in branch models
	return (static_cast<const CModelBase&>(Model())).BranchState().IsBranch();
}

ml::CMaterialLibrary& CMaterialEntry::MaterialLibrary()
{
  assert(m_pMaterialLibrary);
  return *m_pMaterialLibrary;
}

int CMaterialEntry::MaterialModelFilter() const
{
  assert(EntryId() == MD_ROCK_MATERIAL);
  int nFilter =
    MLFO_LINEAR              |
    MLFO_CAMCLAY             |
    MLFO_CREEP               |
    MLFO_MOHRCOULOMB         |
    MLFO_MODIFIEDMOHRCOULOMB |
    MLFO_MC_COHESION_HARD1   |
    MLFO_MC_COHESION_HARD2   |
    MLFO_MC_COHESION_HARD3   |
    MLFO_MC_FRICTION_HARD1   |
    MLFO_MC_FRICTION_HARD2   |
    MLFO_MC_FRICTION_HARD3   |
    MLFO_UNDRAINED           |
    MLFO_RIGIDITY            |
    MLFO_DUALCAP_LINELA      |
    MLFO_FRACTURE_ANISOTROPY |
    MLFO_UPSCALED_ANISOTROPY |
    MLFO_ANISOTROPIC_CAMCLAY |
    MLFO_FRACTURE_APERTURE2  |
    MLFO_CAMCLAY_CREEP;

  return nFilter;
}

ml::CMaterialLibrary& CMaterialEntry::ExternalLibrary()
{
  assert(m_pExternalLibrary);
  return *m_pExternalLibrary;
}

void CMaterialEntry::OnLibraryMaterialSwitched(CLibraryMaterial& libmat, IMaterial& mat)
{
  // replace entry in map
  TMaterialMap::iterator it = m_mpMaterial.find(&libmat);
  if(it != m_mpMaterial.end())
    it->second = &mat;
}

void CMaterialEntry::OnLibraryMaterialAdded(CLibraryMaterial& mat)
{
  // insert corresponding IMaterial object
  IMaterial* pMat = CMaterialFactory::getInstance()->Create(*this, mat);
  pMat->Name(mat.Name());
  m_mpMaterial.insert(TMaterialMap::value_type(&mat, pMat));
  pMat->reParent(this);
}

void CMaterialEntry::OnDeleteLibraryMaterial(CLibraryMaterial& mat)
{
  TMaterialMap::iterator it = m_mpMaterial.find(&mat);
  if(it != m_mpMaterial.end())
  {
    UnLink(*it->second);
    delete it->second;
    m_mpMaterial.erase(it);
  }
}

IMaterial* CMaterialEntry::CorrespondingMaterial(CLibraryMaterial &libmat)
{
  TMaterialMap::iterator it = m_mpMaterial.find(&libmat);
  if(it != m_mpMaterial.end())
    return it->second;

  return 0;
}


///// CMaterialEntry::CMaterialLibraryObserver
CMaterialEntry::CMaterialLibraryObserver::CMaterialLibraryObserver(CMaterialEntry& entry)
: m_entry(entry)
{
  entry.m_pMaterialLibrary->RegisterObserver(*this);
}

CMaterialEntry::CMaterialLibraryObserver::~CMaterialLibraryObserver()
{
  m_entry.m_pMaterialLibrary->UnregisterObserver(*this);
}

void CMaterialEntry::CMaterialLibraryObserver::OnMaterialAdded(ml::CMaterial& mat)
{
  CLibraryMaterial* pLibraryMaterial = dynamic_cast<CLibraryMaterial*>(&mat);
  if(pLibraryMaterial)
    m_entry.OnLibraryMaterialAdded(*pLibraryMaterial);
}

void CMaterialEntry::CMaterialLibraryObserver::OnDeleteMaterial(ml::CMaterial& mat)
{
  CLibraryMaterial* pLibraryMaterial = dynamic_cast<CLibraryMaterial*>(&mat);
  if(pLibraryMaterial)
    m_entry.OnDeleteLibraryMaterial(*pLibraryMaterial);
}
