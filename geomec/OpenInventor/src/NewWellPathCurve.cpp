#include "stdafx.h"

#include "NewWellPathCurve.h"
#include "vcbfunctions.h"
#include "NewWellPath.h"
#include "ITensorGroupTemplate.h"
#include "FieldFactor.h"

void patch_01( CNewWellPath::TPointValueVector& pointvalue_v, OIV2DWellPathGraph::DepthAxis depth_axis )
{
  struct myclass {
    myclass( OIV2DWellPathGraph::DepthAxis param ) { this->param = param; }
    bool operator() ( CNewWellPath::TPointValue p1, CNewWellPath::TPointValue p2 ) //typedef std::pair<CNewWellPoint, geo::CValue> TPointValue;
    { 
      switch( param )
      {
        case OIV2DWellPathGraph::DEPTH_AXIS_AHD:	return( p1.first.TMD() < p2.first.TMD() );
        case OIV2DWellPathGraph::DEPTH_AXIS_TVD:	return( p1.first.TVD() < p2.first.TVD() );
        case OIV2DWellPathGraph::DEPTH_AXIS_Z:		return( p1.first.Z() < p2.first.Z() );
        default:									return( p1.first.TMD() < p2.first.TMD() ); // ERROR, FIXME
      }
    }
    OIV2DWellPathGraph::DepthAxis param;
  };

  std::sort( pointvalue_v.begin(), pointvalue_v.end(), myclass( depth_axis ) ); // complexity: N*log2(N)
}

void NewWellPathCurve::updatePoints()
{
  m_wellPointValueVector.clear();

  const_cast<CNewWellPath *>(m_wellPath)->MapValues(*valueComponent(), unit(), m_wellPointValueVector);

  int i = 0;
  point.deleteValues(0, -1);

  patch_01( m_wellPointValueVector, getDepthAxis() ); // bug 347857

  for (CNewWellPath::TPointValueVector::iterator it = m_wellPointValueVector.begin(); it != m_wellPointValueVector.end(); ++it)
  {
  const CNewWellPoint& p = it->first;
  const geo::CValue& value = it->second;
  if (value.Valid())
  {
      double depthValue = 0.0f;
      
      switch(getDepthAxis())                     
      {
    case OIV2DWellPathGraph::DEPTH_AXIS_AHD:
          depthValue = p.TMD();
          break;
    case OIV2DWellPathGraph::DEPTH_AXIS_TVD:
          depthValue = p.TVD();
          break;
    case OIV2DWellPathGraph::DEPTH_AXIS_Z:
          depthValue = p.Z();
          break;
      }

      float x = (float)value.Value();
      float y = -(float)CLengthQuantity(depthValue).Value(unit());

      point.set1Value(i++, SbVec2f(x, y));
  }
  }
}

void NewWellPathCurve::onUnitChanged()
{
  updatePoints();
}

NewWellPathCurve::NewWellPathCurve(const CNewWellPath* wellPath, const IValueComponentBase* valueComponent, CUnitNode::TUnitType unit, int colorIndex)
  : WellPathCurveBase(valueComponent, unit, colorIndex)
  , m_wellPath(wellPath)
{
  updatePoints();
}

QString NewWellPathCurve::getTextRepresentation() const
{
  QString clip = QString("Wellpath      : %1\r\n").arg(m_wellPath->Name());
    
  const IResultComponent *resultComponent = dynamic_cast<const IResultComponent*>(valueComponent());
  if(resultComponent && resultComponent->AnalysisType().Valid())
  {
    clip += QString("Timestep      : %1\r\n").arg(resultComponent->Stage().Name());
    clip += QString("Analysis type : %1\r\n").arg(resultComponent->AnalysisType().Label());
  }

  clip += "\r\n";

  QString lineFormat("%1\t%2\t%3\t%4\t%5\t%6\r\n");
  double unitFactor = (unit() == IQuantityDouble::FIELD_UNIT) ? FF_FACTOR_LENGTH : 1.0;

  // Start result with header
  clip += QString("Northing\tEasting\tDepth\tAHD\tTVDKB\t%1\r\n").
      arg(getValueComponentLabel());

  if (m_wellPath != 0)
  {
      for (CNewWellPath::TPointValueVector::const_iterator it = m_wellPointValueVector.begin(); it != m_wellPointValueVector.end(); ++it)
    {
          const CNewWellPoint& p = it->first;
      const geo::CValue& value = it->second;
      if(value.Valid())
      {
        QString line = lineFormat
          .arg(p.X() * unitFactor, 0,'f')
          .arg(p.Y() * unitFactor, 0,'f')
          .arg(p.Z() * unitFactor, 0,'f')
          .arg(p.TMD() * unitFactor, 0,'f')
          .arg(p.TVD() * unitFactor, 0,'f')
          .arg(value.Representation().c_str());

        clip += line;
      }
    }
  }

  clip += "\r\n";

  return clip;
}

QString NewWellPathCurve::getTextRepresentation(std::vector<const NewWellPathCurve *>& curves)
{
  QString clip;

  if (curves.empty())
  return clip;

  const NewWellPathCurve *curve = curves[0];

  clip = "Wellpath      : " + curve->m_wellPath->Name() + "\r\n";

  const IResultComponent *resultComponent = 0;
  for (size_t i = 0; i < curves.size() && !resultComponent; ++i)
  {
  resultComponent = dynamic_cast<const IResultComponent*>(curves[i]->valueComponent());
  }

  if (resultComponent && resultComponent->AnalysisType().Valid())
  {
  clip += QString("Timestep      : %1\r\n").arg(resultComponent->Stage().Name());
  clip += QString("Analysis type : %1\r\n").arg(resultComponent->AnalysisType().Label());
  }

  clip += "\r\n";

  QString lineBeginningFormat("%1\t%2\t%3\t%4\t%5");

  double unitFactor = (curve->unit() == IQuantityDouble::FIELD_UNIT) ? FF_FACTOR_LENGTH : 1.0;

  // Start result with header
  clip += QString("Easting\tNorthing\tDepth\tAHD\tTVDKB");

  for (size_t i = 0; i < curves.size(); ++i)
  clip += "\t" + curves[i]->getValueComponentLabel();

  clip += "\r\n";

  std::vector<CNewWellPath::TPointValueVector::const_iterator> pointLists(curves.size());

  size_t NrOfPoints = curve->m_wellPointValueVector.size();

  for (size_t i = 0; i < curves.size(); ++i)
  {
  pointLists[i] = curves[i]->m_wellPointValueVector.begin();
  assert(curves[i]->m_wellPointValueVector.size() == NrOfPoints);
  }

  QString lastLine;

  for (size_t j = 0; j < NrOfPoints; ++j)
  {
  const CNewWellPoint& p = pointLists[0]->first;
  QString line = lineBeginningFormat
      .arg(p.Y() * unitFactor, 0, 'f')
      .arg(p.X() * unitFactor, 0, 'f')
      .arg(p.Z() * unitFactor, 0, 'f')
      .arg(p.TMD() * unitFactor, 0, 'f')
      .arg(p.TVD() * unitFactor, 0, 'f');

  bool haveValid = false;
  for (size_t i = 0; i < curves.size(); ++i)
  {
      line += "\t";

      geo::CValue value = curves[i]->m_wellPointValueVector[j].second;
      if (value.Valid())
      {
    haveValid = true;

    line += QString("%1").arg(value.Value(), 0, 'G', 12);
      }

      ++pointLists[i];
  }

  line += "\r\n";

  if (haveValid)
  {
      if (line != lastLine)
      {
    clip += line;
    lastLine = line;
      }
  }
  }

  clip += "\r\n";

  return clip;

}

void NewWellPathCurve::refresh()
{
  updatePoints();
}

QString NewWellPathCurve::getWellPathName() const
{
  return m_wellPath->Name();
}

const CNewWellPath* NewWellPathCurve::getWellPath() const
{
  return m_wellPath;
}
