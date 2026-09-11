// MaterialEntry.h: interface for the CMaterialEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATERIALENTRY_H__1B1CAFB2_7DE4_412C_B525_A5EA3BB928D3__INCLUDED_)
#define AFX_MATERIALENTRY_H__1B1CAFB2_7DE4_412C_B525_A5EA3BB928D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMaterial.h"
#include "mlMaterialLibrary.h"

class CModelBase;

class CMaterialEntry : public CGraphEntryTemp<IMaterial> {
private:
  typedef CStorageNode::TSTREAM TSTREAM;
  typedef CStorageNode::TPROGRESS TPROGRESS;

public:
  CMaterialEntry(int nEntryId, unsigned int uIconId, const QString &strName, CModelBase &model);
  CMaterialEntry(int nEntryId, unsigned int uIconId, unsigned int uName, CModelBase &model);
  virtual ~CMaterialEntry();
  //	BOOL Append(CMatLib &source);
  //	BOOL Append(CMatLib &source, const QString &sMat);
  //	BOOL Append(CMaterialEntry& source);
  //	const CMatLib& Library() const;
  //	CMatLib& Library();
  //	QString CurrentWorkingDir() const;
  //	IMaterial *Material(const QString &sName);
  //	const IMaterial *Material(const QString &sName) const;
  //	virtual void OnNewNeighbour(const CGraphNode &node);
  //	virtual void OnNeighbourDeleted(const CGraphNode &node);
  IMaterial &CreateMaterial(int iMaterialModel, const QString &strName);
  void DeleteMaterial(IMaterial &material);

  // this function may (will) reshuffle the materials list, so don't call from a loop (or break the loop)
  IMaterial &ConvertMaterial(IMaterial &mat, int iNewMaterialModel);

  void LoadStream(CFemAppModel &model, TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  long SavedItems() const;

  int MaterialSize() const;
  const IMaterial &Material(int i) const;
  IMaterial &Material(int i);

  void ExportToMatLib(const CLibraryMaterial &mat);
  void ImportFromMatLib(const CLibraryMaterial &mat);
  void SetExternalMatLibFileName(const QString &strFileName, bool bImport);
  const QString &GetExternalMatLibFileName() const;
  bool HasValidMatLib() const;
  void UpdateExternalMatLib();

  //	void SetFilterRockMaterial(CMatLib& library);

  bool ReadOnly() const;

  ml::CMaterialLibrary &MaterialLibrary();
  int MaterialModelFilter() const;
  ml::CMaterialLibrary &ExternalLibrary();

  void OnLibraryMaterialSwitched(CLibraryMaterial &libmat, IMaterial &mat);

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialEntry);

private:
  class CMaterialLibraryObserver : public ml::CMaterialLibrary::IObserver {
  public:
    CMaterialLibraryObserver(CMaterialEntry &entry);
    virtual ~CMaterialLibraryObserver();
    // called just after the material was added
    virtual void OnMaterialAdded(ml::CMaterial &mat);
    // called just before the material is deleted
    virtual void OnDeleteMaterial(ml::CMaterial &mat);

  private:
    CMaterialEntry &m_entry;
  };

  friend class CMaterialLibraryObserver;

private:
  void OnLibraryMaterialAdded(CLibraryMaterial &mat);
  void OnDeleteLibraryMaterial(CLibraryMaterial &mat);
  IMaterial *CorrespondingMaterial(CLibraryMaterial &libmat);
  void ReadExternalMatLib();

private:
  typedef std::map<CLibraryMaterial *, IMaterial *> TMaterialMap;
  TMaterialMap m_mpMaterial;

  ml::CMaterialLibrary *m_pMaterialLibrary;
  CMaterialLibraryObserver *m_pLibraryObserver;

  // the material library for import and export
  ml::CMaterialLibrary *m_pExternalLibrary;
  QString m_strExternalLibraryFileName;
};

typedef CSingleCommandTemplate<CMaterialEntry> TMaterialEntryCommand;

#endif // !defined(AFX_MATERIALENTRY_H__1B1CAFB2_7DE4_412C_B525_A5EA3BB928D3__INCLUDED_)
