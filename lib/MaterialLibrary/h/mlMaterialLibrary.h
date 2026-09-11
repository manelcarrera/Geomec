#ifndef _MLMATERIALLIBRARY_H_
#define _MLMATERIALLIBRARY_H_

#include "mlMaterial.h"

#include <set>
#include <vector>

namespace ml {

class CMaterialLibrary {
public:
  class IObserver {
  public:
    virtual ~IObserver() {}
    // called just after the material was added
    virtual void OnMaterialAdded(CMaterial &mat) = 0;
    // called just before the material is deleted
    virtual void OnDeleteMaterial(CMaterial &mat) = 0;
  };

public:
  CMaterialLibrary();
  ~CMaterialLibrary();

  int MaterialSize() const;
  const CMaterial &Material(size_t i) const;
  CMaterial &Material(size_t i);

  CMaterial &AddMaterial(const QString &strName, CMaterial::CCreator &creator);
  void AddMaterial(CMaterial &mat);
  void DeleteMaterial(CMaterial &material);

  void Clear();

  void RegisterObserver(IObserver &observer);
  void UnregisterObserver(IObserver &observer);

private:
  void NotifyMaterialAdded(CMaterial &mat);
  void NotifyDeleteMaterial(CMaterial &mat);

private:
  std::vector<CMaterial *> m_vcMaterials;
  typedef std::vector<IObserver *> TObserverVector;
  TObserverVector m_vcObservers;
};

} // namespace ml

#endif // _MLMATERIALLIBRARY_H_
