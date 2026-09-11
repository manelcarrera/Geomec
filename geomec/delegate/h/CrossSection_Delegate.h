#ifndef _CrossSection_Delegate_h_
#define _CrossSection_Delegate_h_

class CCrossSection;
class CCrossSectionEntry;

#include "StorageNode_Delegate.h"
#include "OpenGLNode_Delegate.h"
#include "CrossSection.h"

class CCrossSection_Delegate : public CStorageNode_Delegate
{
public:
  class CCSObject_Delegate : public COpenGLNode_Delegate
  {
  public:
  CCSObject_Delegate(CCrossSection::CCSObject* csObject);

  private:
  CCSObject_Delegate(const CCSObject_Delegate& rhs);
  CCSObject_Delegate& operator = (const CCSObject_Delegate& rhs);

  CCrossSection::CCSObject* m_csObject;

  REGISTER_DELEGATE(CCrossSection::CCSObject, CCSObject_Delegate);
  };

  class CCSFormation_Delegate : public CCSObject_Delegate
  {
  public:
  CCSFormation_Delegate(CCrossSection::CCSFormation* csFormation);

  private:
  CCSFormation_Delegate(const CCSFormation_Delegate& rhs);
  CCSFormation_Delegate& operator = (const CCSFormation_Delegate& rhs);

  CCrossSection::CCSFormation* m_csFormation;

  REGISTER_DELEGATE(CCrossSection::CCSFormation, CCSFormation_Delegate);
  };

  class CCSWellPath_Delegate : public CCSObject_Delegate
  {
  public:
  CCSWellPath_Delegate(CCrossSection::CCSWellPath* csWellPath);

  private:
  CCSWellPath_Delegate(const CCSWellPath_Delegate& rhs);
  CCSWellPath_Delegate& operator = (const CCSWellPath_Delegate& rhs);

  CCrossSection::CCSWellPath* m_csWellPath;

  REGISTER_DELEGATE(CCrossSection::CCSWellPath, CCSWellPath_Delegate);
  };

  CCrossSection_Delegate(CCrossSection* crossSection);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

private:
  CCrossSection_Delegate(const CCrossSection_Delegate& rhs);
  CCrossSection_Delegate& operator = (const CCrossSection_Delegate& rhs);

  CCrossSection* m_crossSection;

  REGISTER_DELEGATE(CCrossSection, CCrossSection_Delegate);
};

class CCrossSectionEntry_Delegate :
  public CStorageNodeEntry_Delegate <CCrossSection>
{
public:
  CCrossSectionEntry_Delegate(CCrossSectionEntry* crossSectionEntry);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  bool CanInsertNew() const;

  void InsertGraphical();
  void InsertAny();

private:
  CCrossSectionEntry_Delegate(const CCrossSectionEntry_Delegate& rhs);
  CCrossSectionEntry_Delegate& operator = (
  const CCrossSectionEntry_Delegate& rhs);

  CCrossSectionEntry* m_crossSectionEntry;

  REGISTER_DELEGATE(CCrossSectionEntry, CCrossSectionEntry_Delegate);
};

#endif  // _CrossSection_Delegate_h_
