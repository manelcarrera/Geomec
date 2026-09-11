// WellSectionDrawSpec.h: interface for the CWellSectionDrawSpec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLSECTIONDRAWSPEC_H__0DC4C948_970D_4912_8239_22BCB44EFF17__INCLUDED_)
#define AFX_WELLSECTIONDRAWSPEC_H__0DC4C948_970D_4912_8239_22BCB44EFF17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WellDrawSpecBase.h"

namespace well {
  class CWellPoint;
  class IWellSection;
}

namespace geo {
  class IObject;
  class CLine;
}

class CColorScale;




class CWellSectionDrawSpec : public CWellDrawSpecBase
{
  Q_OBJECT

friend class CWellSceneInterMed;

public:


  enum TYPE
  {
    LINE,
    SQUARE,
    ARROW,
    GRADIENT
  };

  enum GRADIENT_TYPE
  {
    TVD,
    TMD
  };

  void SetArrowDistanceLevel(int level);
  int GetArrowDistanceLevel();

  void SetArrowLineAngle(double);

  TYPE GetType() const;
  void SetType(TYPE type);

  void SetColorScaleForGradient(const CColorScale& cs);
  void SetGradientType(GRADIENT_TYPE type);

  virtual ~CWellSectionDrawSpec();

  //returns a new created wellpoint depending on selected objects in vec
  //if no selection then return null
  //caller is responsible for deleting return point
  well::CWellPoint* ProcessNewPointSelection(std::vector<const geo::IObject*>& vec, const geo::CLine& selection_line );


private slots:
  void OnSectionDestroyed(const well::IWellSection& section);

private:
  virtual void CreateScene();

  void CreateLines();
  void CreateSquares();
  void CreateArrow();
  void CreateGradient();

  CWellSectionDrawSpec(CWellSceneInterMed& WellSceneInterMed, well::IWellSection& WellSection, TYPE type = LINE);
  
  well::IWellSection* m_pWellSection;
  

  double m_ArrowLineAngle;
  int m_ArrowDistanceLevel;
    
  CColorScale* m_ColorScale;
  TYPE m_Type;
  GRADIENT_TYPE m_GradientType;

};






#endif // !defined(AFX_WELLSECTIONDRAWSPEC_H__0DC4C948_970D_4912_8239_22BCB44EFF17__INCLUDED_)
