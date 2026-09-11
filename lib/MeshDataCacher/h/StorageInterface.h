#ifndef _STORAGE_INTERFACE_H_
#define _STORAGE_INTERFACE_H_

#include "Checksum.h"

namespace mdc {

class CDataCell;

class IStorageInterface {
public:
  IStorageInterface();
  virtual ~IStorageInterface();

  virtual const char *Directory() const = 0;
  virtual void Directory(const char *directory) = 0;

  virtual bool ReadBlock(CDataCell &cell) = 0;
  virtual bool WriteBlock(CDataCell &cell) = 0;

  virtual bool OwnedByGuard() const = 0;
  virtual void OwnedByGuard(bool bOwnedByGuard) = 0;
};

class CStorageInterfaceDefault : public IStorageInterface {
  char *m_dir;
  bool m_dir_exists;

public:
  CStorageInterfaceDefault(const char *directory = 0);
  virtual ~CStorageInterfaceDefault();

  virtual const char *Directory() const;
  virtual void Directory(const char *directory);

  virtual bool ReadBlock(CDataCell &cell);
  virtual bool WriteBlock(CDataCell &cell);

  virtual bool OwnedByGuard() const;
  virtual void OwnedByGuard(bool bOwnedByGuard);

private:
  char *FileNameExists(int block);
  char *FileName(int block);
  bool DirExists();
};

} // namespace mdc

#endif