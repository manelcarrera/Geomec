#include <string>
#include <sstream>
#include <QRegExp>
#include <QString>
#include <assert.h>
#include "TextTagUtils.h"
#include "DepletionStage.h"
#include "ModelBase.h"
#include "resourceIDS.h"
#include "AnalysisType.h"

const CAnalysisType AnalysisType(const std::string &atype)
{
  CAnalysisType analysisType;
  if ( atype.length() > 1 )
  analysisType.FromExportCharacter( atype[1]);

  return analysisType;
}

const std::string StripQuotes(const std::string& string)
{
  std::string stripped(string);

  stripped.erase(remove(stripped.begin(), stripped.end(), '\''),
  stripped.end());

  return stripped;
}

std::string SurfaceStressComponentList()
{
  return "Normal|ShearPlane|ShearPlaneDipAngle";
}

std::string WellPathStressComponentList()
{
  return "Axial|RadialMax|RadialMin";
}

std::string StrainInvariantResultComponentList()
{
  std::stringstream stream;

  stream << getStringTableEntry(IDS_ET_TENSOR_INV_VOLUME);
  stream << "|";
  stream << getStringTableEntry(IDS_ET_TENSOR_INV_VOLUME);
  stream << "|";
  stream << getStringTableEntry(IDS_ET_TENSOR_INV_2);
  stream << "|";
  stream << getStringTableEntry(IDS_ET_TENSOR_INV_3);
  stream << "|";
  stream << getStringTableEntry(IDS_ET_TENSOR_INV_VON_MISES);

  return stream.str();
}
std::string StressInvariantResultComponentList()
{
  std::stringstream stream;

  stream << getStringTableEntry(IDS_ET_TENSOR_INV_PRESSURE);
  stream << "|";
  stream << getStringTableEntry(IDS_ET_TENSOR_INV_2);
  stream << "|";
  stream << getStringTableEntry(IDS_ET_TENSOR_INV_3);
  stream << "|";
  stream << getStringTableEntry(IDS_ET_TENSOR_INV_VON_MISES);
  stream << "|";
  stream << getStringTableEntry(IDS_ET_TENSOR_INV_TRESCA);
  stream << "|";
  stream << getStringTableEntry(IDS_ET_TENSOR_INV_MEAN);
  stream << "|";
  stream << getStringTableEntry(IDS_ET_TENSOR_INV_VON_MISES_GRADIENT);
  stream << "|";
  stream << getStringTableEntry(IDS_ET_TENSOR_INV_MEAN_GRADIENT);

  return stream.str();
}

std::string VectorResultComponentList()
{
  std::stringstream stream;

  stream << getStringTableEntry(IDS_ET_VECTOR_X);
  stream << "|";
  stream << getStringTableEntry(IDS_ET_VECTOR_Y);
  stream << "|";
  stream << getStringTableEntry(IDS_ET_VECTOR_Z);
  stream << "|";
  stream << getStringTableEntry(IDS_ET_VECTOR_LENGTH);
  stream << "|";
  stream << getStringTableEntry(IDS_ET_VECTOR_FULL);

  return stream.str();
}

std::string TensorResultComponentList()
{
  std::stringstream stream;

  stream << getStringTableEntry( IDS_ET_TENSOR_XX );
  stream << "|";
  stream << getStringTableEntry( IDS_ET_TENSOR_YY );
  stream << "|";
  stream << getStringTableEntry( IDS_ET_TENSOR_ZZ );
  stream << "|";
  stream << getStringTableEntry( IDS_ET_TENSOR_XY );
  stream << "|";
  stream << getStringTableEntry( IDS_ET_TENSOR_YZ );
  stream << "|";
  stream << getStringTableEntry( IDS_ET_TENSOR_XZ );
  stream << "|";
  stream << getStringTableEntry( IDS_ET_TENSOR_FT);

  return stream.str();
}

// waij TFS 92432
/// \brief  split texttag into name basetag and timestep
/// \returns true if a valid split could be made
///
bool SimpleSplitTextTag
( const std::string &textTag //!< tag to be split 'aap_D1'
, std::string &baseTag //!< the base tag 'aap'
, int &timeStepIndex //!< timestep '1'
)
{
  std::string dummy;

  timeStepIndex= -1;

  return 
  SplitTextTag
  ( textTag
  , ""
  , ""
  , baseTag
  , dummy
  , timeStepIndex
  , dummy
  );
}

// waij TFS 92432
/// \brief  split texttag into name basetag, component, timestep and analysis type
/// \returns true if a valid split could be made
///
bool SplitTextTag
( const std::string &textTag //!< tag to be split 'aapN_D1_L'
, const std::string  &componentList //!< in: | separated components N|B|NOOT|F 
, const std::string  &analysisTypeList//!< in: | separated analysis types _C|_L 
, std::string &baseTag //!< the base tag 'aap'
, std::string  &component //!< component 'N'
, int &timeStepIndex //!< timestep '1'
, std::string &analysisType //!< analysis type '_L'
)
{
  QString qTextTag( textTag.c_str() );
  /*bool foundBaseTag= false;*/
  size_t timeStepIndexSize= 0;
  bool bOk= true;

  baseTag.clear();
  component.clear();
  analysisType.clear();
  timeStepIndex= -1;
  
  if ( ! componentList.empty() )
  {
  QRegExp compRegExp(
      QString("(")
      +QString(componentList.c_str())
      +QString(")($|_)")
      );

  if (compRegExp.indexIn(qTextTag) != -1)
  {
      int pos= -1;
      if ( (pos= compRegExp.pos(1)) != -1 )
      {
    baseTag= qTextTag.left(pos).toStdString();
    component= compRegExp.cap(1).toStdString();
      }
  }
  }
  
  // _D followed by one or more digits 
  const QRegExp tsRegExp(QString("_D(\\d{1,})"));

  if (tsRegExp.indexIn(qTextTag) != -1)
  {
  int pos= -1;
  if ( (pos= tsRegExp.pos(1)) != -1 )
  {
      bool hasTimeStepIndex= false;
      timeStepIndexSize= tsRegExp.cap(1).toStdString().size()+2; //_D
      timeStepIndex= tsRegExp.cap(1).toInt(&hasTimeStepIndex);
      bOk= bOk && hasTimeStepIndex;
      if (baseTag.empty())
      {
    baseTag= qTextTag.left(pos-2).toStdString(); //_D
      }
  }
  }

  if ( !analysisTypeList.empty() )
  {
  const QRegExp atRegExp(
      QString("(")
      +QString(analysisTypeList.c_str())
      +QString(")$")
      );

  if (atRegExp.indexIn(qTextTag) != -1)
  {
      int pos= -1;
      if ( (pos= atRegExp.pos(1)) != -1 )
      {
    if ( baseTag.empty() )
    {
          baseTag= qTextTag.left(pos).toStdString();
    }
    analysisType= atRegExp.cap(1).toStdString();
      }
  }
  }

  if ( baseTag.empty() )
  baseTag= textTag;

  if ( bOk )
  bOk= textTag.size() ==
  ( baseTag.size()
  + component.size()
  + timeStepIndexSize
  + analysisType.size() );
  
  return bOk;
}

