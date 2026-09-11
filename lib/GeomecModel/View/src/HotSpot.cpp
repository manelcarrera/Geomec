// HotSpot.cpp: implementation of the CHotSpot class.
//
//////////////////////////////////////////////////////////////////////

#include "HotSpot.h"
#include "FemAppModel.h"
#include "FemAppEntryTypes.h"
#include "resourceIDI.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHotSpot::CHotSpot(unsigned int uName, CFemAppModel &model, TColor min_color, TColor max_color)
:  IColorScaleNode(uName, model), m_MinMaxValue(0, 0), m_nMaxColor(max_color), m_nMinColor(min_color), m_bCenterIsTransparent(true)
{
  // Link to the entry
  assert(Model().GraphEntry(MD_BASE_COLOR_SCALE));
  LinkTo(*Model().GraphEntry(MD_BASE_COLOR_SCALE));	
}

CHotSpot::CHotSpot(CFemAppModel &model) 
: IColorScaleNode(model),  
  m_MinMaxValue(0, 0),
  m_nMaxColor(qRgb(255, 0, 0)),
  m_nMinColor(qRgb(0, 0, 255)),
  m_bCenterIsTransparent(true)
{
}

CHotSpot::CHotSpot(const CHotSpot &rhs)
: IColorScaleNode(rhs),
m_MinMaxValue(rhs.m_MinMaxValue),
m_nMaxColor(rhs.MaxColor()),
m_nMinColor(rhs.MinColor()),
m_bCenterIsTransparent(rhs.CenterIsTransparent())
{
}

CHotSpot::~CHotSpot()
{

}

bool CHotSpot::operator ==(const CHotSpot &rhs) const
{
  if(!IColorScaleNode::operator==(rhs))
    return false;

  return(m_bCenterIsTransparent == rhs.CenterIsTransparent() &&
       m_MinMaxValue == rhs.m_MinMaxValue &&
       m_nMaxColor == rhs.MaxColor() &&
       m_nMinColor == rhs.MinColor());
}

CHotSpot& CHotSpot::operator =(const CHotSpot &rhs)
{
  IColorScaleNode::operator=(rhs);

  m_bCenterIsTransparent = rhs.CenterIsTransparent();
  m_MinMaxValue = rhs.MinMaxValue();
  m_nMaxColor = rhs.MaxColor();
  m_nMinColor = rhs.MinColor();

  return *this;
}


CHotSpot::TColor CHotSpot::Color(const TValue& value, const TMinMax& min_max) const
{
  // Are we in error state?
  if(value.Valid() && min_max.first.Valid() && min_max.second.Valid())
  {
    if(m_bCenterIsTransparent)
    {
      if(value.Value() < m_MinMaxValue.first) 
        return m_nMinColor; // dVal less than minimum value, return the minimum color...
      else if(value.Value() > m_MinMaxValue.second)
        return m_nMaxColor; // dVal more than maximum value, return the maximum color...
      else 
        return qRgba(0, 0, 0, 0); // dVal in between the minimum and maximum values, return transparent...
    }
    else
    {
      if(value.Value() >= m_MinMaxValue.second)
        return m_nMaxColor; // dVal more than maximum value, return the maximum color...
      else if(value.Value() < m_MinMaxValue.first)
        return qRgba(qRed(m_nMinColor), qGreen(m_nMinColor), qBlue(m_nMinColor), 0); // dVal less than the minimum value, return transparent
      else // dVal in bewtween the minimum and maximum values, return calculated color...
      {
        double factor = (value.Value() - m_MinMaxValue.first) / (m_MinMaxValue.second - m_MinMaxValue.first);
        return qRgb(qRed(m_nMaxColor) * factor + qRed(m_nMinColor) * (1 - factor),
              qGreen(m_nMaxColor) * factor + qGreen(m_nMinColor) * (1 - factor),
              qBlue(m_nMaxColor) * factor + qBlue(m_nMinColor) * (1 - factor));
      }
    }
  }
  
  // Return Error color
  assert(false);
  return qRgba(0, 0, 0, 0); // and return black to prevent compiler warning
}

void CHotSpot::SetMaxMinColors(const TColor &nMaxColor, const TColor &nMinColor)
{
  m_nMaxColor = nMaxColor;
  m_nMinColor = nMinColor;
}

void CHotSpot::SetMinMaxValue(const double& dMin, const double& dMax)
{
  m_MinMaxValue.first = dMin;
  m_MinMaxValue.second = dMax;
}

unsigned int CHotSpot::IconId() const
{
  return IDI_COLOR_GRADIENT;
}

unsigned int CHotSpot::TypeId() const
{
  return 0;
}

bool CHotSpot::Empty() const
{
  return !IsLinkedTo(*((CFemAppModel&)Model()).GraphEntry(MD_BASE_COLOR_SCALE));
}

void CHotSpot::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  assert(Empty());
  // Load the base
  IColorScaleNode::LoadStream(stream, version, progress);

  // Load the mode
  int centerIsTransparent;

  stream >> centerIsTransparent;
  m_bCenterIsTransparent = centerIsTransparent;

  // Load the values
  stream >> m_MinMaxValue.first;
  progress.Step();
  stream >> m_MinMaxValue.second;
  progress.Step();

  m_nMinColor = LoadColor(stream);
  progress.Step();
  m_nMaxColor = LoadColor(stream);
  progress.Step();

  // Link to entry
  LinkTo(*Model().GraphEntry(MD_BASE_COLOR_SCALE));
}

void CHotSpot::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  // Save the base
  IColorScaleNode::SaveStream(stream, progress);

  // Load the mode
  int centerIsTransparent = m_bCenterIsTransparent;

  stream << centerIsTransparent;

  // Load the values
  stream << m_MinMaxValue.first;
  progress.Step();
  stream << m_MinMaxValue.second;
  progress.Step();

  SaveColor(stream, m_nMinColor);
  progress.Step();
  SaveColor(stream, m_nMaxColor);
  progress.Step();
}

long CHotSpot::SavedItems() const
{
  return CStorageNode::SavedItems() + 4;
}

const std::pair<double, double>& CHotSpot::MinMaxValue() const
{
  return m_MinMaxValue;
}
