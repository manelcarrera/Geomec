#if !defined(AFX_IDCMATERIAL_H__3E680BB6_D5C7_4282_AE5B_151759C41EBD__INCLUDED_)
#define AFX_IDCMATERIAL_H__3E680BB6_D5C7_4282_AE5B_151759C41EBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>

namespace dia {

class IDianaRunner;

class IElementProperty  
{
public:
	class CElementPropertyLess
	{
	public:
		bool operator()(const IElementProperty *mat1, const IElementProperty *mat2) const
		{
			return *mat1 < *mat2;
		}
	};

	typedef CElementPropertyLess CMaterialLess;

	typedef std::map<const IElementProperty *, int, CElementPropertyLess> TElementPropertyMap;
  typedef std::map<const IElementProperty *, int> TElementPropertyPointerMap;

	IElementProperty();
	virtual ~IElementProperty();

	// write the material assuming the current dir is index of /MATERI or /INPUT/MATERI
	virtual bool WriteFilos(IDianaRunner& diarunner) const = 0;

	virtual bool operator<(const IElementProperty &rhs) const;

  // for distributed properties; once subclasses have been implemented, these should be abstract
  // Type should distinguish between different materials, etc
  // Size is total number of (distributed) parameters in the material
  // Name eg YOUNG, SATURA(3)
  // Values expects a pointer within allocated memory of size stride * Size, to which the values should be written, all in one call
  //   typical use: for all elements 0 <= i < #Elts, get values 0 <= j < Size, call
  //     element[i].IElementProperty.WriteFilosParamValues(..., values + Size * i, 1) // [ Elt 0's value 0 ... Size, Elt 1's value 0 ... Size, etc ]
  //   or
  //     element[i].IElementProperty.WriteFilosParamValues(..., values + i, #Elts) // [ Elt 0 ... #Elts' value 0, Elt 0 ... #Elts' value 1, etc ]
  //   note: if determining Size turns out to be relatively expensive, we can add it as an argument
  //   Rationales for doing it like this:
  //   a. more or less the same as materials currently write to Filos, all values in one call
  //   b. some values, like matrices, are calculated all at once, and it would otherwise be cumbersome to spread it out over multiple calls
  //   c. stride == #Elts is expected to be faster for additional work in the calling routine
  virtual int Type() const;
  virtual int WriteFilosParamSize(IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(IDianaRunner& diarunner, double *values, int stride = 1) const;

  virtual bool isInterfaceElementMaterial() const;
};

typedef IElementProperty IMaterial;
typedef IElementProperty IElementGeometry;
typedef IElementProperty IElementData;

}

#endif // !defined(AFX_IDCMATERIAL_H__3E680BB6_D5C7_4282_AE5B_151759C41EBD__INCLUDED_)
