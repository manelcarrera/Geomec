// ColorGradient.cpp: implementation of the CColorGradient class.
//
//////////////////////////////////////////////////////////////////////

#include <sstream>
#include <limits>
#include <cmath>

#include "ColorGradient.h"
#include "FemAppEntryTypes.h"
#include "resourceIDI.h"
#include "FemAppModel.h"
#include "ISettings.h"
#include "StreamVersion.h"

#ifndef WIN32
#define _isnan isnan
#endif  // WIN32

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
CColorGradient::CColorGradient(CFemAppModel &model)
: IColorScaleNode(model),
  m_bGlobal(true),
  m_bSystemColorGradient(false),
  m_bAutoExtremes(true)
{
  // Used for loading
}

//wjrx mantis 2560
//c'tor for system global color gradient.
//Parameter <name> must match one of the predefined color ranges
//
CColorGradient::CColorGradient(CFemAppModel &model, const QString &name)
: IColorScaleNode(model),
  m_bGlobal(true),
  m_bSystemColorGradient(true),
  m_bAutoExtremes(true)
{
  Name(name);
  Index(666); // apparently not used
  m_MinMaxValue.first= 0; // matches the colormap range
  m_MinMaxValue.second= 600;
  if ( name == "rainbow")
  {
    m_mpColor.insert(TColorMap::value_type(0,   0xffff0000));
    m_mpColor.insert(TColorMap::value_type(100, 0xffff7f00));
    m_mpColor.insert(TColorMap::value_type(200, 0xffffff00));
    m_mpColor.insert(TColorMap::value_type(300, 0xff00ff00));
    m_mpColor.insert(TColorMap::value_type(400, 0xff1e90ff));
    m_mpColor.insert(TColorMap::value_type(500, 0xff4b0082));
    m_mpColor.insert(TColorMap::value_type(600, 0xfff700b5));
  }
  else if ( name == "rainbow_rev.")
  {
    m_mpColor.insert(TColorMap::value_type(0,   0xfff700b5));
    m_mpColor.insert(TColorMap::value_type(100, 0xff4b0082));
    m_mpColor.insert(TColorMap::value_type(200, 0xff1e90ff));
    m_mpColor.insert(TColorMap::value_type(300, 0xff00ff00));
    m_mpColor.insert(TColorMap::value_type(400, 0xffffff00));
    m_mpColor.insert(TColorMap::value_type(500, 0xffff7f00));
    m_mpColor.insert(TColorMap::value_type(600, 0xffff0000));
  }
  else if ( name == "4_bit_dark")
  {
    m_mpColor.insert(TColorMap::value_type(0,   0xff000000));
    m_mpColor.insert(TColorMap::value_type(85,  0xff000080));
    m_mpColor.insert(TColorMap::value_type(170, 0xff008000));
    m_mpColor.insert(TColorMap::value_type(255, 0xff008080));
    m_mpColor.insert(TColorMap::value_type(340, 0xff800000));
    m_mpColor.insert(TColorMap::value_type(425, 0xff800080));
    m_mpColor.insert(TColorMap::value_type(510, 0xff808000));
    m_mpColor.insert(TColorMap::value_type(600, 0xffc0c0c0));
  }
  else if ( name == "4_bit_bright")
  {
    m_mpColor.insert(TColorMap::value_type(0,   0xffc0c0c0));
    m_mpColor.insert(TColorMap::value_type(85,  0xff0000ff));
    m_mpColor.insert(TColorMap::value_type(170, 0xff00ff00));
    m_mpColor.insert(TColorMap::value_type(255, 0xff00ffff));
    m_mpColor.insert(TColorMap::value_type(340, 0xffff0000));
    m_mpColor.insert(TColorMap::value_type(425, 0xffff00ff));
    m_mpColor.insert(TColorMap::value_type(510, 0xffffff00));
    m_mpColor.insert(TColorMap::value_type(600, 0xffffffff));
  }
  else if ( name == "4_bit_dark_rev.")
  {
    m_mpColor.insert(TColorMap::value_type(0,   0xffc0c0c0));
    m_mpColor.insert(TColorMap::value_type(85,  0xff808000));
    m_mpColor.insert(TColorMap::value_type(170, 0xff800080));
    m_mpColor.insert(TColorMap::value_type(255, 0xff800000));
    m_mpColor.insert(TColorMap::value_type(340, 0xff008080));
    m_mpColor.insert(TColorMap::value_type(425, 0xff008000));
    m_mpColor.insert(TColorMap::value_type(510, 0xff000080));
    m_mpColor.insert(TColorMap::value_type(600, 0xff000000));
  }
  else if ( name == "blue_red")
  {
    m_mpColor.insert(TColorMap::value_type(0,   0xff000000));
    m_mpColor.insert(TColorMap::value_type(150, 0xff00ffff));
    m_mpColor.insert(TColorMap::value_type(300, 0xff0000ff));
    m_mpColor.insert(TColorMap::value_type(450, 0xffff00ff));
    m_mpColor.insert(TColorMap::value_type(600, 0xffff0000));
  }
  else if ( name == "4_bit_bright_rev.")
  {
    m_mpColor.insert(TColorMap::value_type(0,   0xffffffff));
    m_mpColor.insert(TColorMap::value_type(85,  0xffffff00));
    m_mpColor.insert(TColorMap::value_type(170, 0xffff00ff));
    m_mpColor.insert(TColorMap::value_type(255, 0xffff0000));
    m_mpColor.insert(TColorMap::value_type(340, 0xff00ffff));
    m_mpColor.insert(TColorMap::value_type(425, 0xff00ff00));
    m_mpColor.insert(TColorMap::value_type(510, 0xff0000ff));
    m_mpColor.insert(TColorMap::value_type(600, 0xffc0c0c0));
  }
  else if ( name == "grey_scale_rev.")
  {
    m_mpColor.insert(TColorMap::value_type(0,   0xffffffff));
    m_mpColor.insert(TColorMap::value_type(150, 0xffc0c0c0));
    m_mpColor.insert(TColorMap::value_type(300, 0xff808080));
    m_mpColor.insert(TColorMap::value_type(450, 0xff404040));
    m_mpColor.insert(TColorMap::value_type(600, 0xff000000));
  }
  else if ( name == "red_blue")
  {
    m_mpColor.insert(TColorMap::value_type(0,   0xffff0000));
    m_mpColor.insert(TColorMap::value_type(150, 0xffff00ff));
    m_mpColor.insert(TColorMap::value_type(300, 0xff0000ff));
    m_mpColor.insert(TColorMap::value_type(450, 0xff00ffff));
    m_mpColor.insert(TColorMap::value_type(600, 0xff000000));
  }
  else if ( name == "grey_scale")
  {
    m_mpColor.insert(TColorMap::value_type(0,   0xff000000));
    m_mpColor.insert(TColorMap::value_type(150, 0xff404040));
    m_mpColor.insert(TColorMap::value_type(300, 0xff808080));
    m_mpColor.insert(TColorMap::value_type(450, 0xffc0c0c0));
    m_mpColor.insert(TColorMap::value_type(600, 0xffffffff));
  }
  else
  {
    assert(0);
  }
   
  reParent(Model().GraphEntry(MD_BASE_COLOR_SCALE));
}

CColorGradient::CColorGradient(unsigned int uName, CFemAppModel &model, const double& firstValue, const double& secondValue, TColor min_color, TColor max_color, bool bGlobal)
: IColorScaleNode(uName, model),
  m_bGlobal(bGlobal),
  m_bSystemColorGradient(false),
  m_bAutoExtremes(true)
{
  // If global try to initialize with proper extents
  if(bGlobal)
  {
    // Default initialisation
    m_mpColor.insert(TColorMap::value_type(0, min_color));
    m_mpColor.insert(TColorMap::value_type(100, max_color));
    m_MinMaxValue = std::pair<double, double>(0, 100);

    FindExtents(firstValue, secondValue);
    // Link to the entry
    assert(Model().GraphEntry(MD_BASE_COLOR_SCALE));
    reParent(Model().GraphEntry(MD_BASE_COLOR_SCALE));
  }
  else
  {
    LoadRegistry("ColorScale", "Local");
    reParent(&Model());
  }
  
  UpdateMinMax();
}

CColorGradient::CColorGradient(const CColorGradient &rhs)
: IColorScaleNode(rhs),
  m_mpColor(rhs.m_mpColor),
  m_bGlobal(rhs.m_bGlobal),
  m_MinMaxValue(rhs.m_MinMaxValue),
  m_bSystemColorGradient(rhs.m_bSystemColorGradient), ///wjrx mantis 2560
  m_bAutoExtremes(rhs.m_bAutoExtremes)
{
  assert(!rhs.IsCopy());
}

bool CColorGradient::operator ==(const CColorGradient &rhs) const
{
  if(!IColorScaleNode::operator ==(rhs))
    return false;

  return (m_mpColor == rhs.m_mpColor &&
      m_MinMaxValue.first == rhs.MinMaxValue().first &&
      m_MinMaxValue.second == rhs.MinMaxValue().second &&
      m_bGlobal == rhs.IsGlobal() &&
      m_bSystemColorGradient==rhs.m_bSystemColorGradient && ///wjrx mantis 2560
      m_bAutoExtremes == rhs.m_bAutoExtremes);
}

CColorGradient& CColorGradient::operator =(const CColorGradient &rhs)
{
  IColorScaleNode::operator =(rhs);
  m_mpColor = rhs.m_mpColor;
  m_MinMaxValue = rhs.MinMaxValue();
  m_bGlobal = rhs.IsGlobal();
  m_bSystemColorGradient = rhs.m_bSystemColorGradient; ///wjrx mantis 2560
  m_bAutoExtremes = rhs.m_bAutoExtremes;

  return *this;
}

bool CColorGradient::AddValue(int iNumerator, int iDenominator, TColor iColor)
{
  m_bSystemColorGradient= false; //wjrx mantis 2560
  double dValue = (m_MinMaxValue.second - m_MinMaxValue.first) * (double(iNumerator) / double(iDenominator)) + m_MinMaxValue.first;
  return AddValue(dValue, iColor);
}

bool CColorGradient::AddValue(const double& dValue, TColor iColor)
{
  int nrofsections = m_mpColor.size() - 1;
  if(nrofsections > 10)
    return false; //No more than 10 sections...
  
  if(m_mpColor.insert(TColorMap::value_type(dValue, iColor)).second)
  {
    UpdateMinMax();
    return true;
  }

  return false;
}

bool CColorGradient::RemoveValue(const double &dValue)
{
  m_bSystemColorGradient= false; //wjrx mantis 2560
  // Try to find the value ...
  TColorMap::iterator it = m_mpColor.find(dValue);

  if(it == m_mpColor.end())
    return false;	// Value not found

  // We can't delete the first and the last
  if((it != m_mpColor.begin()) && (it != --m_mpColor.end()))
  {
    m_mpColor.erase(it);
    UpdateMinMax();
    return true;
  }

  return false;
}

bool CColorGradient::ChangeValue(const double &dOldValue, const double &dNewValue)
{
  m_bSystemColorGradient= false; //wjrx mantis 2560
  // Try to find the value ...
  TColorMap::iterator it= m_mpColor.find(dOldValue);

  if(it == m_mpColor.end())
    return false;	// Value not found

  TColor color = it->second;

  VERIFY(RemoveValue(it->first));

  m_mpColor.insert(TColorMap::value_type(dNewValue, color));

  UpdateMinMax();

  return true;
}

bool CColorGradient::ChangeColor(const double &dValue, TColor color)
{
  m_bSystemColorGradient= false; //wjrx mantis 2560
  // Try to find the value ...
  TColorMap::iterator it= m_mpColor.find(dValue);

  if(it == m_mpColor.end())
    return false;	// Value not found

  it->second = color;

  return true;
}

#pragma optimize("", off)
static bool Indeterminate(const double& dValue)
{
  return  dValue != dValue;
}
#pragma optimize("", on)

CColorGradient::TColor CColorGradient::Color(const double &dValue) const
{
  assert(m_mpColor.size() > 1);

  // waij TFS 74560
  // indeterminate dValue
  // http://stackoverflow.com/questions/410853/how-do-you-check-for-infinite-and-indeterminate-values-in-c
  //
  if ( Indeterminate(dValue) )
    return MinColor();

  if ( dValue == std::numeric_limits<double>::infinity())
    return MaxColor();
  if ( dValue == -std::numeric_limits<double>::infinity())
    return MinColor();

  // Clip colors
  if(MinMaxValue().second <= dValue)
    return MaxColor();

  if(MinMaxValue().first >= dValue)
    return MinColor();

  // Find interval
  color_iterator it_max = m_mpColor.upper_bound(dValue);
  color_iterator it_min = it_max;
  it_min--;

  assert(it_min->first < it_max->first);

  double factor = (dValue - it_min->first) / (it_max->first - it_min->first);

  return qRgb(qRed(it_max->second) * factor + qRed(it_min->second) * (1 - factor),
        qGreen(it_max->second) * factor + qGreen(it_min->second) * (1 - factor),
        qBlue(it_max->second) * factor + qBlue(it_min->second) * (1 - factor));
}

void CColorGradient::SetMinMaxValue(const double& min, const double& max)
{
  if (min == DBL_UNDEFINED || max == DBL_UNDEFINED)
    return;

  double dMin = min;
  double dMax = max;

  assert(m_mpColor.size() > 1);

  double epsilon = DBL_EPSILON * 100.0 * fabs(dMin); // this should be okay for dealing with the ratios below

  if (epsilon < DBL_EPSILON * 100.0)
    epsilon = DBL_EPSILON * 100.0;

  if (fabs(dMax - dMin) < epsilon)
  {
      epsilon *= .5;

      dMin -= epsilon;
      dMax += epsilon;
  }

  assert(dMin < dMax);

  // We make a copy of the color scale and clear the source color map
  // we scale the old values to the source map
  TColorMap mpColor = m_mpColor;
  m_mpColor.clear();
  for(TColorMap::const_iterator it = mpColor.begin(); it != mpColor.end(); it++)
  {
    // Take the old color 
    double dRatio = (it->first - MinMaxValue().first) / (MinMaxValue().second - MinMaxValue().first);
    VERIFY(m_mpColor.insert(TColorMap::value_type(dMin + dRatio * (dMax - dMin), it->second)).second);
  }

  UpdateMinMax();
}

CColorGradient::color_iterator CColorGradient::Begin() const
{
  return m_mpColor.begin();
}

CColorGradient::color_iterator CColorGradient::End() const
{
  return m_mpColor.end();
}

size_t CColorGradient::NumColors() const
{
  return m_mpColor.size();
}

bool CColorGradient::FindExtents(const double& firstValue, const double& secondValue)
{
  if(firstValue == secondValue)
  {
    SetMinMaxValue(firstValue, secondValue + 1);
  }
  else
  {
    SetMinMaxValue(firstValue, secondValue);
  }

  return true;
}

double CColorGradient::RealValue(int &iNumerator, int &iDenomenator)
{
  assert(iDenomenator != 0);
  //a "fraction" is passed in integers. e.g. to pass 52% iNumerator = 52 and iDenomenator = 100...
  return ((MinMaxValue().second - MinMaxValue().first) * (double(iNumerator) / double(iDenomenator)));
}


CColorGradient::TColor CColorGradient::Color(const TValue& value, const TMinMax& min_max) const
{
  if(value.Valid() && min_max.first.Valid() && min_max.second.Valid())
  {
    // Everything is valid in here
    if(m_bGlobal)
    {
      // Globalisten
      return Color(value.Value());
    }

    // Calculate factor ....
    double percentage = ((value.Value() - min_max.first.Value()) / (min_max.second.Value() - min_max.first.Value())) * 100.0;

    return Color(percentage);
  }

  return qRgba(0, 0, 0, 0); // and return black to prevent compiler warning
}

double CColorGradient::Fraction(color_iterator it) const
{
  return (((*it).first - MinMaxValue().first) / (MinMaxValue().second - MinMaxValue().first));
}

 //##ModelId=3BC55D640164
CColorGradient::~CColorGradient()
{

}

bool CColorGradient::AutoExtremes() const
{
  return m_bAutoExtremes;
}

void CColorGradient::AutoExtremes(bool bAuto)
{
  m_bAutoExtremes = bAuto;
}

CColorGradient::TColorMap CColorGradient::colorMap() const
{
  return m_mpColor;
}

void CColorGradient::colorMap(const TColorMap& colorMap)
{
  m_mpColor = colorMap;
  UpdateMinMax();
}

//##ModelId=3BC55D640155
int CColorGradient::MaxColor() const
{
  assert(m_mpColor.size() > 1);
  color_iterator it = m_mpColor.end();
  it--;
  return it->second;
}

//##ModelId=3BC55D640154
int CColorGradient::MinColor() const
{
  assert(m_mpColor.size() > 1);
  return m_mpColor.begin()->second;
}

unsigned int CColorGradient::IconId() const
{
  return IDI_COLOR_GRADIENT;
}

//##ModelId=3BC55D64013F
unsigned int CColorGradient::TypeId() const
{
  return 0;
}

const std::pair<double, double>& CColorGradient::MinMaxValue() const 
{
  return m_MinMaxValue;
}

void CColorGradient::UpdateMinMax()
{
  assert(m_mpColor.size() > 1);

  m_MinMaxValue.first = m_mpColor.begin()->first;
  m_MinMaxValue.second = (--m_mpColor.end())->first;
}

long CColorGradient::SavedItems() const
{
  return IColorScaleNode::SavedItems() + m_mpColor.size();
}

bool CColorGradient::Empty() const
{
  assert(((CFemAppModel&)Model()).GraphEntry(MD_BASE_COLOR_SCALE));
  return !IsLinkedTo(*((CFemAppModel&)Model()).GraphEntry(MD_BASE_COLOR_SCALE));
}

void CColorGradient::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  reParent(0); // make sure it's not connected to the entry
  m_mpColor.clear();
//	assert(Empty());
//	assert(m_mpColor.size() == 0);
  assert(Model().GraphEntry(MD_BASE_COLOR_SCALE));

  // Do base load
  IColorScaleNode::LoadStream(stream, version, progress);

  // Load size
  int nSize;
  stream >> nSize;

  for(int i = 0; i < nSize; i++)
  {
    double dValue;
    stream >> dValue;
    TColor color = LoadColor(stream);

    m_mpColor.insert(TColorMap::value_type(dValue, color));
    progress.Step();
  }

  if(version >= CStreamVersion(3, 7, 7))
  {
  int iAutoExtremes;
  stream >> iAutoExtremes;
  m_bAutoExtremes = (iAutoExtremes != 0);
  }

  // Do the min max
  UpdateMinMax();

  // Link to the entry
  reParent(Model().GraphEntry(MD_BASE_COLOR_SCALE));
}

void CColorGradient::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  assert(Index() > 0);	// Don't forget to number us
  IColorScaleNode::SaveStream(stream , progress);

  // Save our color scale settings
  int nSize = m_mpColor.size();
  stream << nSize;

  for(color_iterator it = m_mpColor.begin(); it != m_mpColor.end(); it++)
  {
    stream << it->first;	// Save value
    SaveColor(stream, it->second);
    progress.Step();
  }

  stream << int(m_bAutoExtremes ? 1 : 0);
}

void CColorGradient::SaveRegistry(const QString& strSection, const QString& strEntry)
{	
  std::stringstream stream;
  // Save our color scale settings
  int nSize = m_mpColor.size();
  stream << nSize << " ";

  for(color_iterator it = m_mpColor.begin(); it != m_mpColor.end(); it++)
  {
    stream << it->first << " ";	// Save value
    int nRed = qRed(it->second);
    int nGreen = qGreen(it->second);
    int nBlue = qBlue(it->second);
    stream << nRed << " ";
    stream << nGreen << " ";
    stream << nBlue << " ";
  }

  ISettings* registry = ISettings::instance();

  registry->writeProfileString(strSection, strEntry, stream.str().c_str());
}

void CColorGradient::LoadRegistry(const QString& strSection, const QString& strEntry)
{	
//	assert(Empty());
  assert(m_mpColor.size() == 0);
//	assert(Model().GraphEntry(MD_BASE_COLOR_SCALE));

  ISettings* registry = ISettings::instance();

  //Nr Val R  G  B Val  R  G  B ...
  //wjrx mantis 2574
  QString sDefault = "7 0  255   0   0 17 255 127   0 33 255 255   0 50   0 255   0 67  30 144 255 83  76   0 130 100 246  0 197";
  QString strColor = registry->getProfileString(strSection, strEntry, sDefault);

  // Parse the shit
  std::stringstream stream(strColor.toStdString());
  // Load size
  int nSize;
  stream >> nSize;

  for(int i = 0; i < nSize; i++)
  {
    double dValue;
    stream >> dValue;
    int nRed, nGreen, nBlue;
    stream >> nRed;
    stream >> nGreen;
    stream >> nBlue;

    m_mpColor.insert(TColorMap::value_type(dValue, qRgb(nRed, nGreen, nBlue)));
  }

  // Do the min max
  UpdateMinMax();


}
