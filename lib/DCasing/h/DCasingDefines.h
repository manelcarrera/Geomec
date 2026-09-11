#ifndef DCASINGDEFINES_HH
#define DCASINGDEFINES_HH

const long LEFTALIGN = 50;
const long VALUESIZE = 15;
#include "QuantityInclude.h"
#include <QTextStream>
#include <qstring.h>
#include <sstream>
#include <string>

const QString DCASING_INPUT_HEADER =
    QString("#######################################################################################\n") +
    QString("########################### ANALYSIS OF THE COMPACTION SAMPLE #########################\n") +
    QString("#######################################################################################");

const QString DCASING_INPUT_VERSION_HEADER = "VERSION INFO";
const QString DCASING_INPUT_VERSION_VERSION = "Version";
const QString DCASING_INPUT_VERSION_REVISION = "Revision";

const QString DCASING_INPUT_WELLDATA_HEADER = "WELL DATA";
const QString DCASING_INPUT_NUMBER_OF_WELLPOINTS = "Number of well points";

const QString DCASING_INPUT_GEOMETRY_HEADING = "GEOMETRY";
const QString DCASING_INPUT_PIPE_LENGTH = "Length of the pipe to be modeled";
const QString DCASING_INPUT_PIPE_STEEL_OUTER = "Pipe steel outer diameter";
const QString DCASING_INPUT_PIPE_STEEL_THICKNESS = "Pipe steel thickness";
const QString DCASING_INPUT_PIPE_STEEL_JOINT_SPACING = "Pipe joint length";
const QString DCASING_INPUT_MIN_REQ_DRIFT_DIAM = "Minimum required drift diameter ";
const QString DCASING_INPUT_DRIFTING_DIAMETER = "Original drift diameter ";
const QString DCASING_INPUT_OUTER_INNER_RADIUS_RATIO = "OD/ID of the support cylinder";

const QString DCASING_INPUT_MATERIAL_HEADING = "MATERIAL";
const QString DCASING_INPUT_YIELD_STRAIN_PIPE_STEEL = "Yield strain of the pipe steel";
const QString DCASING_INPUT_POISSON_RATIO = "Poisson's ratio of the pipe steel";

const QString DCASING_INPUT_LINEAR_COEFF_OF_PIPE = "Linear coeff. of the pipe S-E curve";
const QString DCASING_INPUT_POWER_COEFF_OF_PIPE = "Power coeff.  of the pipe S-E curve";
const QString DCASING_INPUT_HARDENING_COEFF_OF_PIPE = "Hardening coeff.  of the pipe S-E curve";
const QString DCASING_INPUT_STEEL_PIPE_YOUNGS_MODULUS = "Young's modulus of the pipe steel";
const QString DCASING_INPUT_STEEL_PIPE_FRICTION_COEF = "Frict. coeff. between pipe and surrounding";

const QString DCASING_INPUT_DEFORMATION_THEORY = "Buckling theory: DEFOR or FLOW";
const QString DCASING_INPUT_CEMENT_YOUNGS_MODULUS = "Young's modulus of the cement";
const QString DCASING_INPUT_POISSON_RATIO_OF_CEMENT = "Poisson's ratio of the cement";

const QString DCASING_INPUT_LOADING_HEADING = "LOADING";
const QString DCASING_INPUT_MAX_AXIAL_COMPR_STRAIN = "Maximum axial compressive strain (> epsy)";
const QString DCASING_INPUT_LAT_BOUNDARYCONITION = "Outer support lateral boundary condition";
const QString DCASING_INPUT_LOADING_NOFORMATION = "No formation: cement only";

const QString DCASING_INPUT_SOLUTION_PROCESS_PARAM = "SOLUTION PROCESS PARAMETERS";
const QString DCASING_INPUT_TOTAL_NR_OF_INCREMENTS = "Total number of increments";
const QString DCASING_INPUT_MAX_CIRCUMFER_WAVE_NR = "Maximum circumferential wave number";
const QString DCASING_INPUT_MAX_NR_AXIAL_HALF_WAVES = "Maximum number of axial half-waves";

const QString DCASING_INPUT_NON_UNIFORM_HEADING = "NON UNIFORM PARAMETER";
const QString DCASING_INPUT_WELL_POINT = "#Well_p";
const QString DCASING_INPUT_SUP_MOD = "Sup_Mod";
const QString DCASING_INPUT_SUP_POIS = "Sup_Pois";
const QString DCASING_INPUT_PRESSURE_IN = "P_int";
const QString DCASING_INPUT_PRESSURE_EX = "P_ext";
const QString DCASING_INPUT_EPS_COM = "Eps_com";

const long MAXLINELENGTH = 0xFF;
const QString POINTDESCRIPTION = "Well_p";
const QString HEADERDESCRIPTION = "#################################################";
const QString RESULTHEADERDESCRIPTION = "#";

const QString SUPPORTEDHEADERTITLE = "SUPPORTED CASING";
const QString UNSUPPORTEDHEADERTITLE = "UNSUPPORTED CASING";

#define AXISYMMETRIC_SHELL_BUCKLING 0
#define SHELL_COLUMN_BUCKLING 1
#define CRITICAL_FREE_LENGTH 2
#define SHELL_OVALISATION_BUCKLING 3

//////////////////////////////////////////////////////////////////////////////
// method :		ConstructItem
// parameters :	(in) Description:	the description of the first part of the sentence
//				(in) Value:			the value for the sentence
//				(in) Units:			the units for the value (QString for QT support)
// description:	method construcs a string based on description - value - units for the input file
// return :		the string for the inputfile
//////////////////////////////////////////////////////////////////////////////

template <typename T> QString ConstructItem(const QString &Description, const T &Value, const QString &Units = "") {
  return QString("%1").arg(Description, -LEFTALIGN) + QString("%1").arg(QString("%1").arg(Value), VALUESIZE) +
         QString(" %1").arg(Units);
}

//////////////////////////////////////////////////////////////////////////////
// method :		ConstructItem
// parameters :	(in) Description:	the description of the first part of the sentence
//				(in) Value:			the value for the sentence
//				(in) Units:			the units for the value (std::string for the support of the Quantity Units)
// description:	method construcs a string based on description - value - units for the input file
// return :		the string for the inputfile
//////////////////////////////////////////////////////////////////////////////

template <typename T> QString ConstructItem(const QString &Description, const T &Value, const std::string &Units) {
  QString QUnits(Units.c_str());
  return ConstructItem(Description, Value, QUnits);
}

QString ConstructItem(const QString &Description, bool Value);

//////////////////////////////////////////////////////////////////////////////
// method :		TrimQString
// parameters :	(in/out) varString
// description :	method trims the string on both ends (removes the whitespaces)
//////////////////////////////////////////////////////////////////////////////
void TrimQString(QString &varString);

QString EnsureBackslash(const QString &path);

QString EnsureNoBackslash(const QString &path);

class EBad_Cast {

public:
  EBad_Cast() {};
  ~EBad_Cast() {};
};

template <typename Target, typename Source> Target lexical_cast(Source arg) {
  std::stringstream helper_stream;
  Target result = Target();

  if (!(helper_stream << arg && helper_stream >> result))
    throw EBad_Cast();

  return result;
}

template <> double lexical_cast<double, QString>(QString arg);

template <typename Target, typename Source> Target qtlexical_cast(Source arg) {
  QTextStream helper_stream;
  Target result;

  if (!(helper_stream << arg && helper_stream >> result))
    throw EBad_Cast();

  return result;
}

template <typename Target, typename Source> bool CastValue(Target &target, Source source) {
  try {
    target = lexical_cast<Target>(source);
    return true;
  } catch (EBad_Cast &) {
    return false;
  }
}

#endif