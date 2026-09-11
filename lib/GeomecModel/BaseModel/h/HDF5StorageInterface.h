#ifndef _HDF5_STORAGE_INTERFACE_H_
#define _HDF5_STORAGE_INTERFACE_H_

#include "StorageInterface.h"

#include <vector>

class CHDF5File;

class CHDF5StorageInterface : public mdc::IStorageInterface
{
  CHDF5File *m_h5file;
  int        m_modelIndex;
  int        m_maxStages;
  int        m_maxColumns;
  bool       m_owned_by_guard;

  const char *m_directory;

  mdc::IStorageInterface *m_fallbackStorage;
  std::vector<bool> m_useFallback;

  std::vector<bool> m_invalid;

  void SplitColumn(int column, int& type, int& stage, int& result);
  void SetupFallback();

public:
  CHDF5StorageInterface(CHDF5File& h5file, int modelIndex, int maxStages, int maxColumns);
  virtual ~CHDF5StorageInterface();

  virtual const char *Directory() const;
  virtual void Directory(const char *directory);

  virtual bool ReadBlock(mdc::CDataCell& cell);
  virtual bool WriteBlock(mdc::CDataCell& cell);

  virtual bool OwnedByGuard() const;
  virtual void OwnedByGuard(bool bOwnedByGuard);

  void Invalidate(int nAnalysisType);

  void AttachHDF5File(CHDF5File *h5file);

};


#endif