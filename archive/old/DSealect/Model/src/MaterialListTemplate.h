// MaterialListTemplate.h: interface for the CMaterialListTemplate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATERIALLISTTEMPLATE_H__814F8D8E_ADA4_428E_9074_07BB0B0EDFB9__INCLUDED_)
#define AFX_MATERIALLISTTEMPLATE_H__814F8D8E_ADA4_428E_9074_07BB0B0EDFB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//##ModelId=3C60D57300CC
template<class T>
class CMaterialListTemplate
{
protected:
	//##ModelId=3CAAA664039E
	std::vector<T> m_MatVec;
public:
	//##ModelId=3C60D5730158
	typedef typename std::vector<T>::iterator iterator;
	//##ModelId=3C60D5730168
	typedef typename std::vector<T>::const_iterator const_iterator;

	//##ModelId=3C60D573011A
	int NrOfMaterials() const;
	//##ModelId=3C60D573011C
	BOOL Exists(CString name) const;
	//##ModelId=3C60D573011F
	void Add(const T& material);
	//##ModelId=3C60D5730129
	void Remove(const int Index);
	//##ModelId=3C60D573012B
	void Remove(T *material);
	//##ModelId=3C60D5730139
	void Remove(CString name);
	//##ModelId=3C60D573013B
	void RemoveAll();
	//##ModelId=3C60D573013C
	T* GetMaterialAt(int Index);
	const T* GetMaterialAt(int Index) const;
	//##ModelId=3C60D573014A
	T* GetMaterial(CString name);
	const T* GetMaterial(CString name) const;

	//##ModelId=3C60D573014C
	int GetIndex(const T *material) const;
};


//##ModelId=3C60D573011A
template<class T>
int CMaterialListTemplate<T>::NrOfMaterials() const
{
	return m_MatVec.size();
}

//##ModelId=3C60D573011C
template<class T>
BOOL CMaterialListTemplate<T>::Exists(CString name) const
{
	for(int i=0;i<NrOfMaterials();i++)
	{
		if(m_MatVec[i].Name()==name)
			return TRUE;
	}
	return FALSE;
}

//##ModelId=3C60D573011F
template<class T>
void CMaterialListTemplate<T>::Add(const T& material)
{
	m_MatVec.push_back(material);
}

//##ModelId=3C60D5730129
template<class T>
void CMaterialListTemplate<T>::Remove(const int Index)
{
	m_MatVec.erase(m_MatVec.begin()+Index);
}

//##ModelId=3C60D5730139
template<class T>
void CMaterialListTemplate<T>::Remove(CString name)
{
	T* pT = GetMaterial(name);
	if(pT)
		Remove(GetIndex(pT));
}


//##ModelId=3C60D573012B
template<class T>
void CMaterialListTemplate<T>::Remove(T *material)
{
	for(int i=0;i<NrOfMaterials();i++)
	{
		if(&m_MatVec[i]==material)
		{
			Remove(i);
			return;
		}
	}
	ASSERT(FALSE);
	return;
}

//##ModelId=3C60D573013B
template<class T>
void CMaterialListTemplate<T>::RemoveAll()
{
	m_MatVec.clear();
}

//##ModelId=3C60D573013C
template<class T>
T* CMaterialListTemplate<T>::GetMaterialAt(int Index)
{
	ASSERT(Index>=0 && Index< NrOfMaterials());
	return &m_MatVec[Index];
}

template<class T>
const T* CMaterialListTemplate<T>::GetMaterialAt(int Index) const
{
	ASSERT(Index>=0 && Index< NrOfMaterials());
	return &m_MatVec[Index];
}

//##ModelId=3C60D573014A
template<class T>
T* CMaterialListTemplate<T>::GetMaterial(CString name)
{
	for(int i=0;i<NrOfMaterials();i++)
	{
		if(m_MatVec[i].Name()==name)
		{
			return&m_MatVec[i];
		}
	}
	return NULL;
}

template<class T>
const T* CMaterialListTemplate<T>::GetMaterial(CString name) const
{
  return (const_cast<CMaterialListTemplate<T>*>(this))->GetMaterial(name);
}

//##ModelId=3C60D573014C
template<class T>
int CMaterialListTemplate<T>::GetIndex(const T *material) const
{
	for(int i=0;i<NrOfMaterials();i++)
	{
		if(&m_MatVec[i]==material)
			return i;
	}
	return -1;
}



#endif // !defined(AFX_MATERIALLISTTEMPLATE_H__814F8D8E_ADA4_428E_9074_07BB0B0EDFB9__INCLUDED_)
