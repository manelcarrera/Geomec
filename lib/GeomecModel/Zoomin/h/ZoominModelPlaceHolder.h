#ifndef _ZOOMINMODELPLACEHOLDER_H_
#define _ZOOMINMODELPLACEHOLDER_H_

#include "openglnode.h"
#include "GeomecModelVisitor.h"

class CModelBase;

class CZoominModelPlaceHolder : public COpenGLNode
{
public:
  CZoominModelPlaceHolder(CFemAppModel& model);
  CZoominModelPlaceHolder(CFemAppModel& model, CModelBase& childModel);
  ~CZoominModelPlaceHolder();
  
  virtual bool Destroy()
  {
      UnLinkAll();
      return COpenGLNode::Destroy();
  }

  CModelBase& ChildModel();
  const CModelBase& ChildModel() const;

	virtual int DisplayListSize() const;
	virtual const geo::IObject& DisplayList(int nIndex) const;
	virtual TColor Color() const;

	virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;

	virtual bool Empty() const;
	virtual long SavedItems() const;
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  ACCEPT_GEOMECMODELVISITORS(VisitZoominModelPlaceHolder);

private:
  CModelBase* m_pChildModel;
};

#endif // _ZOOMINMODELPLACEHOLDER_H_
