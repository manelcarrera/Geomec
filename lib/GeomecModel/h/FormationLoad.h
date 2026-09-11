#ifndef _FORMATIONLOAD_H_
#define _FORMATIONLOAD_H_

class CDepletionStage;

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
class CFormationLoad : public CFormationLoadBase<VALUE_TYPE>
{
private:
	CFormationBase&  m_formation;
	CDepletionStage& m_depletion_stage;
	bool			 m_bAutoDelete;
protected:
	typename CFormationLoadBase <VALUE_TYPE> ::eModeType m_eType;

private:
	virtual void LoadComponents(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress) = 0;
	virtual void SaveComponents(TSTREAM& stream, TPROGRESS& progress) = 0;
	virtual CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>& FormationStageLoad(CFormationBase& formation, CDepletionStage& next_stage) = 0;

public:
// Construction ....
	CFormationLoad(CFormationBase &f, CDepletionStage &d);
	CFormationLoad(const CFormationLoad& rhs);
	virtual ~CFormationLoad();

	virtual const QString &Name() const;

// Access to modes
	virtual unsigned int ModeSize() const;							// Returns number of modes > 0
	virtual QString ModeName(unsigned int uMode = 0) const;		// Returns name of the mode 

	// Component interface ...
	virtual unsigned int ComponentSize(unsigned int uMode = 0) const;
	virtual IValueComponentBase& Component(unsigned int uComponent = 0, unsigned int uMode = 0);
	virtual const IValueComponentBase& Component(unsigned int uComponent = 0, unsigned int uMode = 0) const;

// Mode
	typename CFormationLoadBase <VALUE_TYPE> ::eModeType Type() const;
	void Type(typename CFormationLoadBase <VALUE_TYPE> ::eModeType type);

// Constant
	const CONSTANT_TYPE& Constant() const;
	CONSTANT_TYPE& Constant();

// GWC
	const GWC_TYPE& GWC() const;
	GWC_TYPE& GWC();

// Access to depletion stage ...
	const CDepletionStage& DepletionStage() const;
	CDepletionStage& DepletionStage();
	bool AutoDelete() const { return m_bAutoDelete;}
	bool AutoDelete() { return m_bAutoDelete;}

// Access to corresponding formation
	virtual const CFormationBase* Formation() const;
	virtual CFormationBase* Formation();

// Assignment and equal
	bool operator==(const CFormationLoad &rhs) const;
	CFormationLoad& operator=(const CFormationLoad& rhs);

  virtual bool IsSimilar(const CFormationLoad& rhs, bool bCheckDistribution = true) const;

// Grpah interaction 
	virtual unsigned int IconId() const;
	virtual void OnNewNeighbour(const CGraphNode &node);
	virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
	virtual void OnNeighbourDeleted(const CGraphNode &node);
	virtual	bool ConnectItem(const CGraphNode &item);
	virtual bool CanConnectItem(const CGraphNode &item) const;
	virtual bool CanDisconnectItem(const CGraphNode& item) const;
	virtual bool Less(const CGraphNode &node) const;

// Area of definition
	virtual geo::CPoint Min() const;
	virtual geo::CPoint Max() const;

#ifdef _DEBUG
	virtual void AssertValid() const;
#endif //_DEBUG

// Save and load
	virtual long SavedItems() const;
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  virtual bool IsFirstValidDepletionStage() const = 0;
};

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::CFormationLoad(CFormationBase &f, CDepletionStage &d)
: CFormationLoadBase<VALUE_TYPE>(d.Name().toStdString().c_str(), f.Model()), m_formation(f), m_depletion_stage(d), m_bAutoDelete(d.Initial()==true), m_eType(CFormationLoadBase <VALUE_TYPE> ::REPEAT_MODE)
{
}

//##ModelId=3BE7AEBA019B
template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::CFormationLoad(const CFormationLoad& rhs)
: CFormationLoadBase<VALUE_TYPE>(rhs),
  m_formation(rhs.m_formation),
  m_depletion_stage(rhs.m_depletion_stage),
  m_bAutoDelete(rhs.m_bAutoDelete),
  m_eType(rhs.m_eType)
{
	assert(CFormationLoadBase <VALUE_TYPE> ::IsLinkedTo(m_formation));
	assert(CFormationLoadBase <VALUE_TYPE> ::IsLinkedTo(m_depletion_stage));

	// Copy the components
	new REPEATER_TYPE((const REPEATER_TYPE&)IValueComposite::Component(0, CFormationLoadBase <VALUE_TYPE> ::REPEAT_MODE));
	new CONSTANT_TYPE(rhs.Constant());			// Copy constant component

	rhs.AssertValid();
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::~CFormationLoad()
{
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
unsigned int CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::ModeSize() const
{
	return 1;
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
QString CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::ModeName(unsigned int /*uMode*/) const
{
	return "";
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
unsigned int CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::ComponentSize(unsigned int /*uMode*/) const
{
	return 1;
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
IValueComponentBase& CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::Component(unsigned int /*uComponent*/, unsigned int /*uMode*/)
{
	return IValueComposite::Component(0, CFormationLoadBase <VALUE_TYPE> ::REPEAT_MODE);
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
const IValueComponentBase& CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::Component(unsigned int /*uComponent*/, unsigned int /*uMode*/) const
{
	return IValueComposite::Component(0, CFormationLoadBase <VALUE_TYPE> ::REPEAT_MODE);
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
typename CFormationLoadBase <VALUE_TYPE> ::eModeType CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::Type() const
{
	return m_eType;
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
void CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::Type(
	typename CFormationLoadBase <VALUE_TYPE> ::eModeType type)
{
	if(m_eType == type)	
		return;

	CDepletionStage* pCurStage = &DepletionStage();
	CDepletionStage* pInitStage = &((CModelBase&)CFormationLoadBase <VALUE_TYPE> ::Model()).InitialDepletionStage();

	if((pCurStage != pInitStage) && type == CFormationLoadBase <VALUE_TYPE> ::REPEAT_MODE)
	{
		if(!CFormationLoadBase <VALUE_TYPE> ::IsCopy())
		{
			m_eType = CFormationLoadBase <VALUE_TYPE> ::REPEAT_MODE;
			CFormationLoadBase <VALUE_TYPE> ::Modified();
			return;
		}

		m_eType = type;
		return;
	}

	if(!CFormationLoadBase <VALUE_TYPE> ::IsCopy())
	{
		m_eType = type;
		CFormationLoadBase <VALUE_TYPE> ::Modified();
		return;
	}

	m_eType = type;
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
const QString &CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::Name() const
{
	return m_depletion_stage.Name();
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
const CONSTANT_TYPE& CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::Constant() const
{
	return (const CONSTANT_TYPE&) IValueComposite::Component(0, CFormationLoadBase <VALUE_TYPE> ::CONSTANT_MODE);
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
CONSTANT_TYPE& CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::Constant()
{
	return (CONSTANT_TYPE&) IValueComposite::Component(0, CFormationLoadBase <VALUE_TYPE> ::CONSTANT_MODE);
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
const GWC_TYPE& CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::GWC() const
{
	return (const GWC_TYPE&) IValueComposite::Component(0, CFormationLoadBase <VALUE_TYPE> ::GWC_MODE);
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
GWC_TYPE& CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::GWC()
{
	return (GWC_TYPE&) IValueComposite::Component(0, CFormationLoadBase <VALUE_TYPE> ::GWC_MODE);
}

#ifdef _DEBUG
template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
void CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::AssertValid() const
{
	assert(this->IsLinkedTo(DepletionStage()));
	assert(this->IsLinkedTo(*Formation()));
}
#endif //_DEBUG

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
void CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::LoadStream(TSTREAM& stream, CStreamVersion &version,TPROGRESS& progress)
{
	assert(CFormationLoadBase <VALUE_TYPE> ::Empty());

	// Load the base class
	CFormationLoadBase<VALUE_TYPE>::LoadStream(stream, version, progress);

	// Load the type
	int nType;
	stream >> nType;
	m_eType = (typename CFormationLoadBase <VALUE_TYPE> ::eModeType)(nType);

	LoadComponents(stream, version, progress);

	assert(!CFormationLoadBase <VALUE_TYPE> ::Empty());
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
void CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
	// Save base class
	CFormationLoadBase<VALUE_TYPE>::SaveStream(stream,progress);
	int nType = Type();
	stream << nType;

	SaveComponents(stream, progress);
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
long CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::SavedItems() const
{
	return Component().SavedItems() + CFormationLoadBase<VALUE_TYPE>::SavedItems();
}

//##ModelId=3BC55D5F03E2
template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
const CDepletionStage& CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::DepletionStage() const
{
	assert(CFormationLoadBase <VALUE_TYPE> ::IsLinkedTo(m_depletion_stage));
	return m_depletion_stage;
}

//##ModelId=3BC55D5F03E4
template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
CDepletionStage& CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::DepletionStage()
{
	return m_depletion_stage;
}

//##ModelId=3BC55D600005
template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
const CFormationBase* CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::Formation() const
{
	return &m_formation;
}

//##ModelId=3BC55D600007
template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
CFormationBase* CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::Formation() 
{
	return &m_formation;
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
unsigned int CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::IconId() const
{
	return Component(0, CFormationLoadBase <VALUE_TYPE> ::REPEAT_MODE).IconId();
}

//##ModelId=3BC55D60003A
template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
bool CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::operator==(const CFormationLoad &rhs) const
{
	if(!CFormationLoadBase<VALUE_TYPE>::operator ==(rhs))
		return false;

	if(Type() != rhs.Type())
		return false;

	switch(rhs.Type())
	{
		case CFormationLoadBase <VALUE_TYPE> ::CONSTANT_MODE:
			return Constant() ==  rhs.Constant();
		case CFormationLoadBase <VALUE_TYPE> ::REPEAT_MODE:
			return true;
		case CFormationLoadBase <VALUE_TYPE> ::GWC_MODE:
			return GWC() == rhs.GWC();
		default:
			break;
	}



	return false;
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
bool CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::IsSimilar(const CFormationLoad &rhs, bool bCheckDistribution) const
{
  if (Type() != rhs.Type())
    return Type() == CFormationLoadBase<VALUE_TYPE>::REPEAT_MODE; // if true, we just switched to "same as previous"

  if (bCheckDistribution)
  {
    if (this->DistributedSize() != 0)
    {
      if (this->DistributedSize() != rhs.DistributedSize())
        return false;
      if (this->DistributedOnly() != rhs.DistributedOnly())
        return false;
    }
  }
  else if (this->DistributedSize() != rhs.DistributedSize())
    return false;

  for (size_t i = 0; i < this->DistributedSize(); ++i)
  {
    size_t j;

    for (j = 0; j < this->DistributedSize(); ++j)
      if (&this->DistributedValue(i) == &rhs.DistributedValue(j))
        break;

    if (j == this->DistributedSize())
      return false;
  }

  return true;
}


template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>&
CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::operator=(const CFormationLoad& rhs)
{
	CFormationLoadBase <VALUE_TYPE> ::AssertValid();
	rhs.AssertValid();

	CFormationLoadBase<VALUE_TYPE>::operator =(rhs);

	switch(rhs.Type())
	{
		case CFormationLoadBase <VALUE_TYPE> ::CONSTANT_MODE:
			Constant() = rhs.Constant();
			break;
		case CFormationLoadBase <VALUE_TYPE> ::REPEAT_MODE:
			break;
		case CFormationLoadBase <VALUE_TYPE> ::GWC_MODE:
			GWC() = rhs.GWC();
			break;
		default:
			break;
	}

	Type(rhs.Type());

	// Icon of next load may need updating, flag it modified
	CDepletionStage& stage = DepletionStage();
	if(CFormationLoadBase <VALUE_TYPE> ::parent() && !stage.Last())
	{
		CDepletionStage& next_stage = stage.Next();
		CFormationBase& formation = dynamic_cast<CFormationBase&>(*CFormationLoadBase <VALUE_TYPE> ::parent());
		CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>& next_load = FormationStageLoad(formation, next_stage);
		next_load.Modified();
	}

	Component().Modified();

	CFormationLoadBase <VALUE_TYPE> ::AssertValid();
	return *this;
}


//##ModelId=3BC55D600048
template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
void CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::OnNewNeighbour(const CGraphNode &node)
{
	CFormationLoadBase<VALUE_TYPE>::OnNewNeighbour(node);
}

//##ModelId=3BC55D600054
template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
void CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
	if(&node == &m_depletion_stage)
		CFormationLoadBase <VALUE_TYPE> ::Modified();		// Name could be changed ...
	CFormationLoadBase<VALUE_TYPE>::OnNeighbourModified(node, uHint);
}


template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
void CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::OnNeighbourDeleted(const CGraphNode &node)
{
	bool bDelete = false;

	if(&node == Formation())
		bDelete = true;

	if(&node == &m_depletion_stage)
		bDelete = true;

	CFormationLoadBase<VALUE_TYPE>::OnNeighbourDeleted(node);

	if(bDelete)
		delete this;
}



//##ModelId=3BC55D600057
template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
bool CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::ConnectItem(const CGraphNode &item)
{
	assert(CanConnectItem(item));

	const CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>* pFormationLoad =
		dynamic_cast<const CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>*>(&item);

	if(pFormationLoad)
	{
		switch(pFormationLoad->Type())
		{
		case CFormationLoadBase <VALUE_TYPE> ::CONSTANT_MODE:
			Type(CFormationLoadBase <VALUE_TYPE> ::CONSTANT_MODE);
			Constant().Set(	pFormationLoad->Constant().ReferenceValue().Value(),
							pFormationLoad->Constant().ReferenceDepth().Value(),
							pFormationLoad->Constant().Gradient().Value(),
							CQuantity::SI_UNIT);
		break;
		case CFormationLoadBase <VALUE_TYPE> ::REPEAT_MODE:
		case CFormationLoadBase <VALUE_TYPE> ::GWC_MODE:
		default:
			assert(false);
			break;
		}

		return true;
	}
	
 	return CFormationLoadBase<VALUE_TYPE>::ConnectItem(item);
	
}
	
//##ModelId=3BC55D60005A
template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
bool CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::CanConnectItem(const CGraphNode &item) const
{
	const CModelBase& model = static_cast<const CModelBase&>(CFormationLoadBase <VALUE_TYPE> ::Model());

	if(model.BranchState().IsBranch())
	{
		const VALUE_TYPE* pDistri = dynamic_cast<const VALUE_TYPE*>(&item);
		if(pDistri && !DepletionStage().isAfter(model.BranchState().BranchStage()))
			return false;
	}

	if(CFormationLoadBase<VALUE_TYPE>::CanConnectItem(item))
		return true;

	const CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>* pFormationLoad =
		dynamic_cast<const CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>*>(&item);
	
	if(pFormationLoad)
		return !((pFormationLoad->Type() == CFormationLoadBase <VALUE_TYPE> ::REPEAT_MODE) || (pFormationLoad->Type() == CFormationLoadBase <VALUE_TYPE> ::GWC_MODE));

	return false;
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
bool CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::CanDisconnectItem(const CGraphNode& item) const
{
	const CModelBase& model = static_cast<const CModelBase&>(CFormationLoadBase <VALUE_TYPE> ::Model());

	if(model.BranchState().IsBranch())
	{
		const VALUE_TYPE* pDistri = dynamic_cast<const VALUE_TYPE*>(&item);
		if(pDistri)
			return DepletionStage().isAfter(model.BranchState().BranchStage());
	}

	return true;
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
bool CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::Less(const CGraphNode &node) const
{
	const CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>* pFormationLoad =
		dynamic_cast<const CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>*>(&node);

	if(pFormationLoad)
		return	DepletionStage().Less(pFormationLoad->DepletionStage());

	return CGraphNode::Less(node);
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
geo::CPoint CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::Min() const
{
	return Formation()->Min();
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
geo::CPoint CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::Max() const
{
	return Formation()->Max();
}

#endif // _FORMATIONLOAD_H_
