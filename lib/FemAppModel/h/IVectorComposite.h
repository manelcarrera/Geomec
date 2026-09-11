// IVectorComposite.h: interface for the IVectorComposite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IVECTORCOMPOSITE_H__555DBF2F_8994_4D2F_A17F_DFAB15F72EA1__INCLUDED_)
#define AFX_IVECTORCOMPOSITE_H__555DBF2F_8994_4D2F_A17F_DFAB15F72EA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IVectorComposite // : public IValueComposite
{
public:
  class IVectorComponent // : public IValueComponentBase
  {
  public:
    enum COMPONENT { X_COMPONENT = 0, Y_COMPONENT, Z_COMPONENT, LENGTH_COMPONENT };

  private:
    COMPONENT m_component;

  public:
    // Construction
    IVectorComponent(IVectorComposite &parent, enum COMPONENT component, int nMode = 0);
  };

public:
private:
  // Access to the vector ...
  //	virtual geo::CVector ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit) const = 0;
  //	virtual std::vector<geo::CVector> ValueVector(const geo::IElement& element, const CQuantity::UNIT unit) const = 0;

  IVectorComposite();
  virtual ~IVectorComposite();
};

#endif // !defined(AFX_IVECTORCOMPOSITE_H__555DBF2F_8994_4D2F_A17F_DFAB15F72EA1__INCLUDED_)
