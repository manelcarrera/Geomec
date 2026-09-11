 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"

#include "IDrawDef.h"
#include "DrawDef.h"
#include "ColorScale.h"
#include "ColorScaleDrawDef.h"
#include "Point.h"
#include "ILine.h"


CColorScaleDrawDef::CColorScaleDrawDef(const geo::IObject &object)
{
  Init();
  double dZ   = (object.Max().Z()-object.Min().Z());
  double dMin = object.Min().Z();

  AddPair(dMin + 0.00*dZ , qRgb(255,0,0));
  AddPair(dMin + 0.50*dZ , qRgb(255,165,0));
  AddPair(dMin + 1.00*dZ , qRgb(255,255,0));

}

CColorScaleDrawDef::CColorScaleDrawDef()
{
  Init();	
}

void CColorScaleDrawDef::AddPair(double value, TColor color)
{
  m_colorscale.Insert(std::make_pair(value,color));
}

void CColorScaleDrawDef::Clear()
{
  m_colorscale.Clear();
}

CColorScaleDrawDef::CColorScaleDrawDef(double val1,double val2,int color1,int color2)
{
  Init();
  AddPair(val1,color1);
  AddPair(val2,color2);
}

CColorScaleDrawDef::~CColorScaleDrawDef()
{

}

/*COLORREF CColorScaleDrawDef::Color(const geo::IPoint &pt) const
{
  int nColor;
  
  switch(m_eDirection)
  {
  case CS_X_DIRECTION:
    {
      nColor = m_colorscale.GetColor(pt.X());		
      break;
    }
  case CS_Y_DIRECTION:
    {
      nColor = m_colorscale.GetColor(pt.Y());
      break;
    }
  case CS_Z_DIRECTION:
    {
      nColor = m_colorscale.GetColor(pt.Z());
      break;
    }
  default:
    DIA_ASSERT(FALSE);
  }

  return nColor;
}
*/

std::vector<CColorScaleDrawDef::TColor> CColorScaleDrawDef::Color(const geo::IObject &object) const
{
  const geo::IElement *pElement = dynamic_cast<const geo::IElement*>(&object);
  const geo::ILine *pLine		  = dynamic_cast<const geo::ILine*>(&object);
  const geo::IPoint *pPoint	  = dynamic_cast<const geo::IPoint*>(&object);
  
  std::vector<TColor> vcColor;
  std::vector<const geo::IPoint*> vcPoints;
  if(pElement)
  {
    for(int i = 0; i < pElement->NrOfPoints(); i++)
    {
      vcPoints.push_back(&(pElement->Point(i)));
    }
  }
  else if(pLine)
  {
    vcPoints.push_back(&(pLine->First()));
    vcPoints.push_back(&(pLine->Second()));
  }
  else if(pPoint)
  {
    vcPoints.push_back(pPoint);
  }
  else
  {
    DIA_ASSERT(FALSE);
  }

  switch(m_eDirection)
  {
  case CS_X_DIRECTION:
    {
      for(size_t i = 0; i < vcPoints.size(); i++)
      {
        vcColor.push_back(m_colorscale.GetColor(vcPoints[i]->X()));
      }
      break;
    }
  case CS_Y_DIRECTION:
    {
      for(size_t i = 0; i < vcPoints.size(); i++)
      {
        vcColor.push_back(m_colorscale.GetColor(vcPoints[i]->Y()));
      }
      break;
    }
  case CS_Z_DIRECTION:
    {
      for(size_t i = 0; i < vcPoints.size(); i++)
      {
        vcColor.push_back(m_colorscale.GetColor(vcPoints[i]->Z()));
      }
      break;
    }
  default:
    DIA_ASSERT(FALSE);
  }
    
  return vcColor;
}

void CColorScaleDrawDef::Init()
{
  m_bPolyFillFront=TRUE;
  m_bPolyFillBack=TRUE;
  m_bVisible=TRUE;
  m_bLineStipple=FALSE;
  m_fLineWidth=2;
  m_fPointSize=2;
  m_bPolyDrawFront=TRUE;
  m_bPolyDrawBack=TRUE;
  m_iDepthFunc=GL_ALWAYS;
  m_eDirection = CS_Z_DIRECTION;
  m_bCurvedDraw = true;
}

  
